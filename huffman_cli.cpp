#include <algorithm>
#include <array>
#include <chrono>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <memory>
#include <queue>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

struct Node {
    uint64_t freq; int minSymbol; int symbol;
    std::shared_ptr<Node> left, right;
    Node(uint64_t f, int s) : freq(f), minSymbol(s), symbol(s) {}
    Node(std::shared_ptr<Node> a, std::shared_ptr<Node> b)
        : freq(a->freq+b->freq), minSymbol(std::min(a->minSymbol,b->minSymbol)), symbol(-1), left(std::move(a)), right(std::move(b)) {}
};
using Ptr = std::shared_ptr<Node>;
struct Compare { bool operator()(const Ptr& a,const Ptr& b) const { return a->freq!=b->freq ? a->freq>b->freq : a->minSymbol>b->minSymbol; } };
static void codesFor(const Ptr& n,const std::string& path,std::array<std::string,256>& codes){if(!n)return;if(n->symbol>=0){codes[n->symbol]=path.empty()?"0":path;return;}codesFor(n->left,path+"0",codes);codesFor(n->right,path+"1",codes);}
static Ptr buildTree(const std::array<uint64_t,256>& freq){std::priority_queue<Ptr,std::vector<Ptr>,Compare> q;for(int i=0;i<256;i++)if(freq[i])q.push(std::make_shared<Node>(freq[i],i));while(q.size()>1){auto a=q.top();q.pop();auto b=q.top();q.pop();q.push(std::make_shared<Node>(a,b));}return q.empty()?nullptr:q.top();}
static uint64_t hashData(const std::string&s){uint64_t h=14695981039346656037ULL;for(unsigned char c:s){h^=c;h*=1099511628211ULL;}return h;}
static void append64(std::string& out,uint64_t v){for(int i=0;i<8;i++)out.push_back(char(v>>(8*i)));}
static uint64_t read64(std::istream& in){uint64_t v=0;for(int i=0;i<8;i++){int c=in.get();if(c==EOF)throw std::runtime_error("Truncated HuffChat archive.");v|=uint64_t(uint8_t(c))<<(8*i);}return v;}
// Frequencies are usually small. Variable-length integers avoid wasting 8 bytes per symbol.
static void appendVarint(std::string& out,uint64_t value){while(value>=0x80){out.push_back(char((value&0x7f)|0x80));value>>=7;}out.push_back(char(value));}
static uint64_t readVarint(std::istream& in){uint64_t value=0;for(unsigned shift=0;shift<64;shift+=7){int c=in.get();if(c==EOF)throw std::runtime_error("Truncated Huffman frequency table.");uint8_t byte=uint8_t(c);if(shift==63&&(byte&0xfe))throw std::runtime_error("Invalid frequency value.");value|=uint64_t(byte&0x7f)<<shift;if(!(byte&0x80))return value;}throw std::runtime_error("Invalid frequency value.");}
static std::string readFile(const std::string&p){std::ifstream f(p,std::ios::binary);if(!f)throw std::runtime_error("Cannot open input file.");return {std::istreambuf_iterator<char>(f),{}};}
static void writeFile(const std::string&p,const std::string&d){std::ofstream f(p,std::ios::binary);if(!f)throw std::runtime_error("Cannot create output file.");f.write(d.data(),static_cast<std::streamsize>(d.size()));if(!f)throw std::runtime_error("Failed writing output file.");}
static std::string compressData(const std::string& data,uint64_t& bitCount){std::array<uint64_t,256> freq{};for(unsigned char c:data)freq[c]++;auto root=buildTree(freq);std::array<std::string,256> codes{};codesFor(root,"",codes);bitCount=0;for(unsigned char c:data)bitCount+=codes[c].size();std::string payload;payload.reserve((bitCount+7)/8);uint8_t byte=0;int used=0;for(unsigned char c:data)for(char bit:codes[c]){byte=uint8_t((byte<<1)|(bit=='1'));if(++used==8){payload.push_back(char(byte));byte=0;used=0;}}if(used)payload.push_back(char(byte<<(8-used)));
    std::string out="HUF2";append64(out,data.size());append64(out,bitCount);append64(out,hashData(data));uint16_t count=0;for(auto f:freq)if(f)count++;out.push_back(char(count&255));out.push_back(char(count>>8));for(int i=0;i<256;i++)if(freq[i]){out.push_back(char(i));appendVarint(out,freq[i]);}out+=payload;return out;}
static std::string decompressData(const std::string& archive){std::istringstream in(archive,std::ios::binary);char magic[4];in.read(magic,4);if(in.gcount()!=4||std::string(magic,4)!="HUF2")throw std::runtime_error("Not a valid HuffChat .huff file.");uint64_t original=read64(in),bits=read64(in),checksum=read64(in);int lo=in.get(),hi=in.get();if(lo==EOF||hi==EOF)throw std::runtime_error("Truncated HuffChat archive.");uint16_t count=uint16_t(uint8_t(lo)|(uint16_t(uint8_t(hi))<<8));if(count>256)throw std::runtime_error("Invalid Huffman table.");std::array<uint64_t,256> freq{};uint64_t total=0;for(unsigned i=0;i<count;i++){int s=in.get();if(s==EOF)throw std::runtime_error("Truncated Huffman table.");uint64_t f=readVarint(in);if(!f||freq[uint8_t(s)])throw std::runtime_error("Invalid Huffman frequency table.");freq[uint8_t(s)]=f;total+=f;}if(total!=original)throw std::runtime_error("Archive metadata is inconsistent.");if(original>100000000ULL)throw std::runtime_error("File exceeds the 100 MB decompression limit.");if(original==0){if(count||bits)throw std::runtime_error("Invalid empty archive.");return {};}
    auto root=buildTree(freq);if(!root)throw std::runtime_error("Missing Huffman tree.");std::string out;out.reserve(static_cast<size_t>(original));if(root->symbol>=0){if(bits!=original)throw std::runtime_error("Invalid single-symbol archive.");out.assign(static_cast<size_t>(original),char(root->symbol));}else{auto cur=root;uint64_t consumed=0;int c;while(consumed<bits&&(c=in.get())!=EOF){for(int k=7;k>=0&&consumed<bits;k--,consumed++){cur=((uint8_t(c)>>k)&1)?cur->right:cur->left;if(!cur)throw std::runtime_error("Invalid compressed bitstream.");if(cur->symbol>=0){out.push_back(char(cur->symbol));cur=root;if(out.size()>original)throw std::runtime_error("Decoded data exceeds expected size.");}}}if(consumed!=bits||cur!=root||out.size()!=original)throw std::runtime_error("Compressed data is incomplete or corrupted.");}
    if(hashData(out)!=checksum) throw std::runtime_error("Integrity check failed: archive may be corrupted.");
    return out;
}
int main(int argc,char**argv){try{if(argc!=4){std::cerr<<"Usage: huffchat_cli compress|decompress INPUT OUTPUT\n";return 2;}std::string mode=argv[1],data=readFile(argv[2]);if(mode=="compress"){uint64_t bits=0;auto start=std::chrono::steady_clock::now();auto archive=compressData(data,bits);auto end=std::chrono::steady_clock::now();writeFile(argv[3],archive);double ms=std::chrono::duration<double,std::milli>(end-start).count();double saved=data.empty()?0.0:100.0*(1.0-double(archive.size())/double(data.size()));std::cout<<"{\"originalBytes\":"<<data.size()<<",\"logicalBits\":"<<bits<<",\"archiveBytes\":"<<archive.size()<<",\"percentSaved\":"<<saved<<",\"timeMs\":"<<ms<<"}\n";}else if(mode=="decompress"){auto start=std::chrono::steady_clock::now();auto plain=decompressData(data);auto end=std::chrono::steady_clock::now();writeFile(argv[3],plain);std::cout<<"{\"restoredBytes\":"<<plain.size()<<",\"timeMs\":"<<std::chrono::duration<double,std::milli>(end-start).count()<<"}\n";}else throw std::runtime_error("Unknown mode.");return 0;}catch(const std::exception&e){std::cerr<<e.what()<<"\n";return 1;}}
