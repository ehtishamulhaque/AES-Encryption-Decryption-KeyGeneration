//AES
//Key Expension

#include <iostream>  
#include <bitset>  
using namespace std;
typedef bitset<8> byte;
typedef bitset<16> word;

const int Nr = 2;  //S-AES-16 requires 2 rounds of encryption  
const int Nk = 4;   //Nk Represents the number of word s that are input keys  

byte S_Box[4][4] = {
    {0x9, 0x4, 0xA, 0xB},
    {0xD, 0x1, 0x8, 0x5},
    {0x6, 0x2, 0x0, 0x3},
    {0xC, 0xE, 0xF, 0x7}
};

//Round constant, used in key expansion. (AES-128 only takes 10 rounds)  
word Rcon[1] = { 0x80};
word Rcon[2] = { 0x30};

/**
 * Convert four byte s to one word.
 */
word Word(byte& k1, byte& k2)
{
    word result(0x00000000);
    word temp;
    temp = k1.to_ulong();  // K1  
    temp <<= 8;
    result |= temp;
    temp = k2.to_ulong();  // K2  
    //temp <<= 16;
    result |= temp;
    return result;
    
    /*
    temp = k3.to_ulong();  // K3  
    temp <<= 8;
    result |= temp;
    temp = k4.to_ulong();  // K4  
    result |= temp;
    return result;
    */
}

/**
 *  Cyclic left shift by byte
 *  That is to say, [a0, a1, a2, a3] becomes [a1, a2, a3, a0]
 */
word RotWord(word& rw)
{
    word high = rw << 0;
    word low = rw >> 8;
    return high | low;
}

/**
 *  S-box transformation for each byte in input word
 */
word SubWord(word& sw)
{
    word temp;
    for (int i = 0; i < 16; i += 2)
    {
        int row = sw[i + 3] * 2 + sw[i + 2];
        int col =  sw[i + 1] * 2 + sw[i];
        byte val = S_Box[row][col];
        for (int j = 0; j < 4; ++j)
            temp[i + j] = val[j];
    }
    return temp;
}

/**
 *  Key Extension Function - Extended 128-bit key to w[4*(Nr+1)]
 */
void KeyExpansion(byte key[2 * Nk], word w[2 * (Nr + 1)])
{
    word temp;
    int i = 0;
    //The first four of w [] are input key s  
    while (i < Nk)
    {
        w[i] = Word(key[2 * i], key[2 * i + 1]); //, key[4 * i + 2], key[4 * i + 3]);
        ++i;
    }

    i = Nk;

    while (i < 2 * (Nr + 1))
    {
        temp = w[i - 1]; //Record the previous word  
        if (i % Nk == 0)
            w[i] = w[i - Nk] ^ SubWord((temp)) ^ Rcon[i / Nk - 1];
        else
            w[i] = w[i - Nk] ^ temp;
        ++i;
    }
}

int main()
{
    byte key[16] = { 0x2, 0x4,
                    0x7, 0x5 };

    word w[4 * (Nr + 1)];

    cout << "KEY IS: ";
    for (int i = 0; i < 2; ++i)
        cout << hex << key[i].to_ulong() << " ";
    cout << endl;

    KeyExpansion(key, w);
    //Testing  
    for (int i = 0; i < 2 * (Nr + 1); ++i)
        cout << "w[" << dec << i << "] = " << hex << w[i].to_ulong() << endl;

    return 0;
}