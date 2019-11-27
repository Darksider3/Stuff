#include <iostream>

#define TT800N 25
#define TT800M 7
struct TT800Seeds
{
	uint32_t seed1 = 9;
	uint32_t seed2 = 3402;
}TT800_SEEDS;

uint32_t TT800_SEED1;
uint32_t TT800_SEED2;

static void TT800_vec_update(uint32_t* const p);
static void TT800_vec_init(uint32_t* const p, const int length);
uint32_t TT800();

static void TT800_vec_init(uint32_t* const p, const int length)
{
    const uint32_t multi = 509845221ul;
    const uint32_t adder = 3ul;
    for(int i = 0; i < length; ++i)
    {
        TT800_SEEDS.seed1 = TT800_SEEDS.seed1 * multi + adder;
        TT800_SEEDS.seed2 *= TT800_SEEDS.seed2 + 1;
        p[i] = TT800_SEED2 + (TT800_SEEDS.seed1 >> 10);
    }
}

static void TT800_vec_update(uint32_t* const p)
{
    static const uint32_t A[2] = {0, 0x8ebfd028};
    int i = 0;
    for(; i < TT800N - TT800M; ++i)
    {
        p[i] = p[i+(TT800M)] ^ (p[i] >> 1) ^ A[p[i] & 1];
    }
    for(; i < TT800N; ++i)
    {
        p[i] = p[i+(TT800M-TT800N)] ^ (p[i] >> 1) ^ A[p[i] & 1];
    }
}

uint32_t TT800()
{
    static uint32_t vector[TT800N];
    static int idx = TT800N+1;

    uint32_t e;

    if(idx >= TT800N)
    {
        if(idx > TT800N)
        {
            TT800_vec_init(vector, TT800N);
        }
        TT800_vec_update(vector);
        idx = 0;
    }

    e = vector[idx++];
    e ^= (e << 7)  & 0x2b5b2500; 
    e ^= (e << 15) & 0xdb8b0000;
    e ^= (e >> 16);
    return e;
}

int main()
{
	TT800_SEEDS.seed1 = 903ul;
	TT800_SEEDS.seed2 = 12381ul;
	int max = 1000;
	for(int i = 0; i < max; ++i)
	{
		std::cout << std::to_string(TT800()) << ", ";
	}
	std::cout <<  "\n";
}
