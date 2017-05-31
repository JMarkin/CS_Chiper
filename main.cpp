#include <boost/utility/binary.hpp>
#include <stdio.h>
#include <stdlib.h>
#include <bitset>
#include <iostream>
#include <iomanip>
#include <boost/multiprecision/cpp_int.hpp>
#include <vector>
#include <sstream>

using namespace std;
using namespace boost::multiprecision;


const uint64_t _c[2] = {0xb7e151628aed2a6a, 0xbf7158809cf4f3c7};
const uint64_t C[9] = {0x290d61409ceb9e8f, 0x1f855f585b013986, 0x972ed7d635ae1716, 0x21b6694ea5728708,
                       0x3c18e6e7faadb889,
                       0xb700f76f73841163, 0x3f967f6ebf149dac, 0xa40e7ef6204a6230, 0x03c54b5a46a34465};
const uint8_t p[16][16] = {
        {0x29, 0x0d, 0x61, 0x40, 0x9c, 0xeb, 0x9e, 0x8f, 0x1f, 0x85, 0x5f, 0x58, 0x5b, 0x01, 0x39, 0x86},
        {0x97, 0x2e, 0xd7, 0xd6, 0x35, 0xae, 0x17, 0x16, 0x21, 0xb6, 0x69, 0x4e, 0xa5, 0x72, 0x87, 0x08},
        {0x3c, 0x18, 0xe6, 0xe7, 0xfa, 0xad, 0xb8, 0x89, 0xb7, 0x00, 0xf7, 0x6f, 0x73, 0x84, 0x11, 0x63},
        {0x3f, 0x96, 0x7f, 0x6e, 0xbf, 0x14, 0x9d, 0xac, 0xa4, 0x0e, 0x7e, 0xf6, 0x20, 0x4a, 0x62, 0x30},
        {0x03, 0xc5, 0x4b, 0x5a, 0x46, 0xa3, 0x44, 0x65, 0x7d, 0x4d, 0x3d, 0x42, 0x79, 0x49, 0x1b, 0x5c},
        {0xf5, 0x6c, 0xb5, 0x94, 0x54, 0xff, 0x56, 0x57, 0x0b, 0xf4, 0x43, 0x0c, 0x4f, 0x70, 0x6d, 0x0a},
        {0xe4, 0x02, 0x3e, 0x2f, 0xa2, 0x47, 0xe0, 0xc1, 0xd5, 0x1a, 0x95, 0xa7, 0x51, 0x5e, 0x33, 0x2b},
        {0x5d, 0xd4, 0x1d, 0x2c, 0xee, 0x75, 0xec, 0xdd, 0x7c, 0x4c, 0xa6, 0xb4, 0x78, 0x48, 0x3a, 0x32},
        {0x98, 0xaf, 0xc0, 0xe1, 0x2d, 0x09, 0x0f, 0x1e, 0xb9, 0x27, 0x8a, 0xe9, 0xbd, 0xe3, 0x9f, 0x07},
        {0xb1, 0xea, 0x92, 0x93, 0x53, 0x6a, 0x31, 0x10, 0x80, 0xf2, 0xd8, 0x9b, 0x04, 0x36, 0x06, 0x8e},
        {0xbe, 0xa9, 0x64, 0x45, 0x38, 0x1c, 0x7a, 0x6b, 0xf3, 0xa1, 0xf0, 0xcd, 0x37, 0x25, 0x15, 0x81},
        {0xfb, 0x90, 0xe8, 0xd9, 0x7b, 0x52, 0x19, 0x28, 0x26, 0x88, 0xfc, 0xd1, 0xe2, 0x8c, 0xa0, 0x34},
        {0x82, 0x67, 0xda, 0xcb, 0xc7, 0x41, 0xe5, 0xc4, 0xc8, 0xef, 0xdb, 0xc3, 0xcc, 0xab, 0xce, 0xed},
        {0xd0, 0xbb, 0xd3, 0xd2, 0x71, 0x68, 0x13, 0x12, 0x9a, 0xb3, 0xc2, 0xca, 0xde, 0x77, 0xdc, 0xdf},
        {0x66, 0x83, 0xbc, 0x8d, 0x60, 0xc6, 0x22, 0x23, 0xb2, 0x8b, 0x91, 0x05, 0x76, 0xcf, 0x74, 0xc9},
        {0xaa, 0xf1, 0x99, 0xa8, 0x59, 0x50, 0x3b, 0x2a, 0xfe, 0xf9, 0x24, 0xb0, 0xba, 0xfd, 0xf8, 0x55},
};
uint64_t F(const uint64_t c, uint64_t x);

uint64_t P8(uint64_t x);

uint64_t T(uint64_t p8);

template<typename T>
T rol(T x) {
    return (x << 1) | ((x >> (sizeof(x) * CHAR_BIT - 1)) & 1);
}

template<typename T>
T h(T x) {
    return rol(x) & (0x55) ^ x;
}

uint16_t concatenate(uint8_t x, uint8_t y) {
    uint16_t a = (x << ((sizeof(y) * CHAR_BIT)));
    a |= y;
    return a;
}


uint8_t P(uint8_t x) {
    int xl = x >> 4;
    int xr = x & (15);

    return p[xl][xr];
}

uint16_t M(uint16_t x) {
    uint8_t xl = static_cast<uint8_t >(x >> 8);
    uint8_t xr = static_cast<uint8_t >(x);
    uint8_t yl = P(h(xl) ^ xr);
    uint8_t yr = P(rol(xl) ^ xr);
    return concatenate(yl, yr);
}

template<typename T>
T _h(T x) {
    return (rol(x) & (0xaa)) ^ x;
}


uint16_t _M(uint16_t x) {
    uint8_t xl = static_cast<uint8_t >(x >> 8);
    uint8_t xr = static_cast<uint8_t >(x);
    uint8_t yl = _h(P(xl) ^ P(xr));
    uint8_t yr = rol(yl) ^P(xr);
    return concatenate(yl, yr);
}

uint64_t E(uint64_t x) {
    for (int w = 0; w < 3; w++) {
        uint16_t m[4] = {x >> 48, (x >> 32), (x >> 16), x};
        uint64_t l[8];
        for (int i = 0; i < 4; i++) {
            m[i] = M(m[i]);
            l[2 * i] = m[i] >> 8;
            l[2 * i + 1] = (uint8_t) m[i];
        }
        x = l[0] << (8 * 7) | l[2] << (8 * 6) | l[4] << (8 * 5) | l[6] << (8 * 4) |
            l[1] << (8 * 3) | l[3] << (8 * 2) | l[5] << (8 * 1) | l[7];
        if (w == 2)
            return x;
        x ^= _c[w];
    }
}

uint64_t _E(uint64_t x) {
    for (int w = 0; w < 3; w++) {
        uint16_t m1[4] = {x >> 48, (x >> 32), (x >> 16), x};
        uint64_t l[8];
        for (int i = 0; i < 4; i++) {
            l[2 * i] = m1[i] >> 8;
            l[2 * i + 1] = (uint8_t) m1[i];
        }
        x = l[0] << (8 * 7) | l[4] << (8 * 6) | l[1] << (8 * 5) | l[5] << (8 * 4) |
            l[2] << (8 * 3) | l[6] << (8 * 2) | l[3] << (8 * 1) | l[7];
       // cout << bitset<64>(x) << endl;
        uint64_t m[4] = {_M((uint16_t) (x >> 48)), _M((uint16_t) (x >> 32)), _M((uint16_t) (x >> 16)),
                         _M((uint16_t) x)};
        x = m[0] << 48 | m[1] << 32 | m[2] << 16 | m[3];
        //cout << "do xor " << bitset<64>(x) << endl;
        if (w == 2)
            return x;
        x ^= _c[1 - w];
        //cout << "posle " << bitset<64>(x) << endl << endl;
    }
}

vector<uint64_t> gen_key(uint64_t k_1,uint64_t k_2) {
    vector<uint64_t> keys(9);
    keys[0] = (k_2 ^ F(C[0], k_1));
    keys[1] = (k_1 ^ F(C[1], keys[0]));
    for (int i = 2; i < 9; i++)
        keys[i] = (keys[i - 2] ^ F(C[i], keys[i - 1]));
    return keys;
}

uint64_t F(const uint64_t c, uint64_t x) {
    return T(P8(x ^ c));
}

uint64_t T(uint64_t p8) {
    bitset<64> b(p8);
    string p=b.to_string();
    vector<bool> x(p.begin(), p.end());
    bitset<64> l;
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++)
            l[8 * j + i] = x[8 * i + j]=='0'?0:1;
    return l.to_ullong();
}

uint64_t P8(uint64_t x) {
    uint8_t m1[8] = {x >> 56, (x >> 48), (x >> 40), x>>32,x>>24,x>>16,x>>8,x};
    uint64_t l[8];
    for (int i = 0; i < 8; i++) {
        int xl = m1[i] >> 4;
        int xr = m1[i] & (15);
        l[i] = p[xr][xl];
    }
    uint64_t ans = l[0] << (8 * 7) | l[1] << (8 * 6) | l[2] << (8 * 5) | l[3] << (8 * 4) |
                   l[4] << (8 * 3) | l[5] << (8 * 2) | l[6] << (8 * 1) | l[7];
    return ans;
}

uint64_t crypto(uint64_t text,uint64_t key){
    vector<uint64_t > keys=gen_key(key,0);
    text^=keys[0];
    for(int i=1;i<9;i++){
        text=E(text);
        text^=keys[i];
    }
    return text;
}
uint64_t decrypto(uint64_t text,uint64_t key){
    vector<uint64_t > keys=gen_key(key,0);
    text^=keys[8];
    for(int i=7;i>=0;i--){
        text=_E(text);
        text^=keys[i];
    }
    return text;
}

/*
 *  Дальше идет реализация Хэша
 */

string to_binary(const string& input)
{
    ostringstream oss;
    for(auto c : input) {
        oss << std::bitset<8>(c);
    }
    return oss.str();
}

string tostring(const uint64_t& input){
    string bin= bitset<64>(input).to_string();
    int result =0 ;

    for(size_t count = 0; count < bin.length() ; ++count)
    {
        result *=2;
        result += bin[count]=='1'? 1 :0;
    }

    stringstream ss;
    ss << hex << setfill('0')  << result;

    string hexVal(ss.str());
    return hexVal;
}
string my_hash(string S){
    string binS(to_binary(S));
    uint64_t h=511090419505352761121;
    for (unsigned int i = 0; i < binS.length(); i += 64) {
        int k=64;
        if(i+64>=binS.length())
            k=binS.length()-i;
        bitset<64> bs1(binS.substr(i, k));
        uint64_t M1 =bs1.to_ullong();
        h=crypto(M1^h,h)^M1;
    }
    return tostring(h);
}


/*
 *  Реализация Цифровой подпись Эль Гамаля
 *
 */

/*
 * Реализация теста Миллера на простоту
 */
uint64_t mulmod (uint64_t a, uint64_t b, uint64_t p) {
    uint64_t res = 1;
    while (b)
        if (b & 1)
            res = uint64_t (res * 1ll * a % p),  --b;
        else
            a = uint64_t (a * 1ll * a % p),  b >>= 1;
    return res;
}

uint64_t generator_kor (uint64_t p) {
    vector<uint64_t> fact;
    uint64_t phi = p-1,  n = phi;
    for (uint64_t i=2; i*i<=n; ++i)
        if (n % i == 0) {
            fact.push_back (i);
            while (n % i == 0)
                n /= i;
        }
    if (n > 1)
        fact.push_back (n);

    for (uint64_t res=2; res<=p; ++res) {
        bool ok = true;
        for (size_t i=0; i<fact.size() && ok; ++i)
            ok &= mulmod (res, phi / fact[i], p) != 1;
        if (ok)  return res;
    }
    return 0;
}

uint64_t modulo(uint64_t base, uint64_t exponent, uint64_t mod)
{
    uint64_t x = 1;
    uint64_t y = base;
    while (exponent > 0)
    {
        if (exponent % 2 == 1)
            x = (x * y) % mod;
        y = (y * y) % mod;
        exponent = exponent / 2;
    }
    return x % mod;
}

bool Miller(uint64_t p,int iteration)
{
    if (p < 2)
    {
        return false;
    }
    if (p != 2 && p % 2==0)
    {
        return false;
    }
    uint64_t s = p - 1;
    while (s % 2 == 0)
    {
        s /= 2;
    }
    for (int i = 0; i < iteration; i++)
    {
        uint64_t a = rand() % (p - 1) + 1, temp = s;
        uint64_t mod = modulo(a, temp, p);
        while (temp != p - 1 && mod != 1 && mod != p - 1)
        {
            mod = mulmod(mod, mod, p);
            temp *= 2;
        }
        if (mod != p - 1 && temp % 2 == 0)
        {
            return false;
        }
    }
    return true;
}

uint64_t gcd (uint64_t a, uint64_t b) {
    if (b == 0)
        return a;
    else
        return gcd (b, a % b);
}

uint64_t generate(uint64_t max,uint64_t min,bool pr){
    while(1) {
        srand((unsigned) time(NULL));
        uint64_t ch = min + (rand() % static_cast<uint64_t>(max - min+1));
        if(pr){
            if(gcd(ch,max+1)==1)
                return ch;
        }else
            if(Miller(ch,1000))
                return ch;

    }
}

uint64_t binpow (uint64_t a, uint64_t n,uint64_t mod) {
    uint64_t res = 1;
    while (n) {
        if (n & 1)
            res *= a%mod;
        a *= a;
        n >>= 1;
    }
    return res%mod;
}
int gcd1 (int a, int b, int & x, int & y) {
    if (a == 0) {
        x = 0; y = 1;
        return b;
    }
    int x1, y1;
    int d = gcd1 (b%a, a, x1, y1);
    x = y1 - (b / a) * x1;
    y = x1;
    return d;
}
pair<uint64_t ,uint64_t > signature(string S){
    uint64_t p=generate(10132740823261162944,1,false);
    uint64_t g=generator_kor(p);
    srand((unsigned) time(NULL));
    uint64_t x = 1+ (rand() % static_cast<uint64_t>(p));
    uint64_t m;
    stringstream ss;
    ss << hex << my_hash(S);
    ss >> m;
    uint64_t k=generate(p-1,1,true);
    uint64_t r=binpow(g,k,p);
    uint64_t s= ((m-x*r)%(p-1))*(binpow(k,p-2,p-1)) %(p-1);
    return {r,s};
}


int main() {
    cout<<crypto(1292991,78127381321321)<<endl;
    cout<<decrypto(8436550725249456760,78127381321321)<<endl;
    cout<<my_hash("adasdasdakkcksaaakcksakcdasfdasfasfsvdsvsavdsvdsva");
    signature("adasdasdakkcksaaakcksakcdasfdasfasfsvdsvsavdsvdsva");
    return 0;
}