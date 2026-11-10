#!/usr/bin/env python3
"""HuffChat local web server. Uses the C++ Huffman engine in huffman_cli.cpp."""
from http.server import ThreadingHTTPServer, SimpleHTTPRequestHandler
from pathlib import Path
import base64, json, mimetypes, subprocess, tempfile, threading, time
from urllib.parse import unquote

ROOT = Path(__file__).resolve().parent
WEB = ROOT / "web"
CLI = ROOT / "huffchat_cli"
HISTORY = ROOT / "history.csv"
LOCK = threading.Lock()
HISTORY_HEADER = [
    "timestamp", "operation", "filename", "original_bytes",
    "archive_bytes", "logical_bits", "percent_saved", "time_ms"
]

def ensure_history_schema_unlocked():
    """Keep the web history schema consistent without deleting legacy data.

    Older project versions wrote a five-column CSV. Those rows do not contain
    enough information to reconstruct archive sizes or operation names, so
    preserve the old file as a timestamped backup and start a clean web log.
    Caller must hold LOCK.
    """
    if not HISTORY.exists() or HISTORY.stat().st_size == 0:
        with HISTORY.open("w", newline="", encoding="utf-8") as f:
            import csv
            csv.writer(f).writerow(HISTORY_HEADER)
        return

    import csv
    with HISTORY.open(newline="", encoding="utf-8") as f:
        first_row = next(csv.reader(f), [])
    if first_row == HISTORY_HEADER:
        return

    stamp = time.strftime("%Y%m%d-%H%M%S")
    backup = HISTORY.with_name(f"history-legacy-{stamp}.csv")
    suffix = 1
    while backup.exists():
        backup = HISTORY.with_name(f"history-legacy-{stamp}-{suffix}.csv")
        suffix += 1
    HISTORY.replace(backup)
    with HISTORY.open("w", newline="", encoding="utf-8") as f:
        import csv
        csv.writer(f).writerow(HISTORY_HEADER)
    print(f"[HuffChat] Legacy history preserved as {backup.name}; started a new CSV log.")

class Handler(SimpleHTTPRequestHandler):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, directory=str(WEB), **kwargs)

    def log_message(self, fmt, *args):
        print("[HuffChat] " + (fmt % args))

    def send_bytes(self, status, data, content_type="application/octet-stream", extra=None):
        self.send_response(status)
        self.send_header("Content-Type", content_type)
        self.send_header("Content-Length", str(len(data)))
        self.send_header("X-Content-Type-Options", "nosniff")
        for k, v in (extra or {}).items(): self.send_header(k, v)
        self.end_headers()
        self.wfile.write(data)

    def json(self, status, obj):
        self.send_bytes(status, json.dumps(obj).encode(), "application/json; charset=utf-8")

    def do_GET(self):
        if self.path == "/api/history":
            with LOCK:
                ensure_history_schema_unlocked()
                import csv
                with HISTORY.open(newline="", encoding="utf-8") as f:
                    rows=list(csv.DictReader(f))[-50:][::-1]
            return self.json(200, {"items": rows})
        if self.path == "/api/health": return self.json(200, {"ok": True, "engine": "C++17 Huffman"})
        if self.path == "/": self.path = "/index.html"
        return super().do_GET()

    def read_body(self, limit=25*1024*1024):
        try: n=int(self.headers.get("Content-Length", "0"))
        except ValueError: raise ValueError("Invalid Content-Length.")
        if n < 0 or n > limit: raise ValueError("Upload is too large (25 MB limit).")
        data=self.rfile.read(n)
        if len(data)!=n: raise ValueError("Incomplete request body.")
        return data

    def run_cli(self, mode, data):
        with tempfile.TemporaryDirectory(prefix="huffchat-") as td:
            src=Path(td)/"input.bin"; dst=Path(td)/"output.bin"
            src.write_bytes(data)
            p=subprocess.run([str(CLI), mode, str(src), str(dst)], capture_output=True, text=True, timeout=30)
            if p.returncode: raise ValueError(p.stderr.strip() or "Compression engine failed.")
            result=json.loads(p.stdout.strip().splitlines()[-1])
            return dst.read_bytes(), result

    def add_history(self, mode, stats, filename="Text input"):
        with LOCK:
            ensure_history_schema_unlocked()
            with HISTORY.open("a", newline="", encoding="utf-8") as f:
                import csv
                w=csv.writer(f)
                w.writerow([time.strftime("%Y-%m-%d %H:%M:%S"),mode,filename,stats.get("originalBytes",stats.get("restoredBytes",0)),stats.get("archiveBytes",""),stats.get("logicalBits",""),round(stats.get("percentSaved",0),2),round(stats.get("timeMs",0),3)])

    def do_POST(self):
        try:
            if self.path == "/api/compress":
                raw=self.read_body(2*1024*1024)
                archive,stats=self.run_cli("compress",raw)
                stats["archiveBase64"]=base64.b64encode(archive).decode("ascii")
                stats["archiveBytes"]=len(archive)
                self.add_history("Text compression",stats)
                return self.json(200,stats)
            if self.path == "/api/decompress":
                obj=json.loads(self.read_body(25*1024*1024).decode("utf-8"))
                raw=base64.b64decode(obj.get("archiveBase64",""),validate=True)
                plain,stats=self.run_cli("decompress",raw)
                try: text=plain.decode("utf-8")
                except UnicodeDecodeError: return self.json(400,{"error":"The restored data is not valid UTF-8 text. Use file decompression to download the original bytes."})
                return self.json(200,{"text":text,**stats})
            if self.path == "/api/compress-file":
                raw=self.read_body(); name=unquote(self.headers.get("X-Filename","document.txt"))
                if not name.lower().endswith(".txt"): raise ValueError("Please upload a .txt file.")
                archive,stats=self.run_cli("compress",raw); self.add_history("File compression",{**stats,"archiveBytes":len(archive)},name)
                safe=Path(name).name.rsplit(".",1)[0].replace('"','') or "file"
                return self.send_bytes(200,archive,"application/octet-stream",{"Content-Disposition":f'attachment; filename="{safe}.huff"','X-Original-Bytes':str(len(raw)),'X-Archive-Bytes':str(len(archive)),'X-Logical-Bits':str(stats['logicalBits']),'X-Percent-Saved':str(stats['percentSaved']),'X-Time-Ms':str(stats['timeMs'])})
            if self.path == "/api/decompress-file":
                raw=self.read_body(); name=unquote(self.headers.get("X-Filename","file.huff"))
                plain,stats=self.run_cli("decompress",raw); safe=Path(name).name.rsplit(".",1)[0].replace('"','') or "restored"
                return self.send_bytes(200,plain,"text/plain; charset=utf-8",{"Content-Disposition":f'attachment; filename="{safe}.txt"','X-Restored-Bytes':str(len(plain)),'X-Time-Ms':str(stats['timeMs'])})
            return self.json(404,{"error":"Endpoint not found."})
        except (ValueError, json.JSONDecodeError, UnicodeDecodeError, subprocess.TimeoutExpired) as e:
            return self.json(400,{"error":str(e)})
        except Exception as e:
            return self.json(500,{"error":"Server error: "+str(e)})

if __name__ == "__main__":
    if not CLI.exists(): raise SystemExit("huffchat_cli is missing. Run ./build.sh first.")
    print("\n  HuffChat is running at http://127.0.0.1:8000\n  Press Ctrl+C to stop.\n")
    ThreadingHTTPServer(("127.0.0.1",8000),Handler).serve_forever()
