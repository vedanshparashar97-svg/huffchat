
#include "EncoderDecoder.h"
// below is encode
std::string encode(
    const std::string& message,
    const std::unordered_map<char, std::string>& codes)
{
    std::string encoded;

    for (char c : message)
    {
        encoded += codes.at(c);
    }

    return encoded;
}
// below is decode
std::string decode(const std::string& encoded, HuffmanNode* root)
{
    if (encoded.empty() || root == nullptr)
    {
        return "";
    }

    std::string decoded;

    //Special case: if the tree has only 1 character
    if (root->isLeaf())
    {
        for (char bit : encoded)
        {
            if (bit != '0' && bit != '1')
            {
                break;
            }

            decoded += root->ch;
        }

        return decoded;
    }

    HuffmanNode* current = root;

    for (char bit : encoded)
    {
        if (bit == '0')
        {
            current = current->left;
        }
        else if (bit == '1')
        {
            current = current->right;
        }
        else
        {
            break;
        }

        if (current == nullptr)
        {
            break;
        }

        if (current->isLeaf())
        {
            decoded += current->ch;
            current = root;
        }
    }
    return decoded;
}
