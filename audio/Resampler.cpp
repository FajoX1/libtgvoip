//
// Created by Grishka on 01.04.17.
//

#include "Resampler.h"

#include <cmath>
#include <cstring>

using namespace tgvoip::audio;
static constexpr std::int16_t hann[960] =
{
    0x0000, 0x0000, 0x0000, 0x0001, 0x0001, 0x0002, 0x0003, 0x0004, 0x0006, 0x0007, 0x0009, 0x000B, 0x000D, 0x000F, 0x0011, 0x0014, 0x0016, 0x0019, 0x001C, 0x0020,
    0x0023, 0x0027, 0x002A, 0x002E, 0x0033, 0x0037, 0x003B, 0x0040, 0x0045, 0x004A, 0x004F, 0x0054, 0x005A, 0x0060, 0x0065, 0x006B, 0x0072, 0x0078, 0x007F, 0x0085,
    0x008C, 0x0093, 0x009B, 0x00A2, 0x00AA, 0x00B2, 0x00B9, 0x00C2, 0x00CA, 0x00D2, 0x00DB, 0x00E4, 0x00ED, 0x00F6, 0x00FF, 0x0109, 0x0113, 0x011C, 0x0127, 0x0131,
    0x013B, 0x0146, 0x0150, 0x015B, 0x0166, 0x0172, 0x017D, 0x0189, 0x0194, 0x01A0, 0x01AC, 0x01B9, 0x01C5, 0x01D2, 0x01DF, 0x01EC, 0x01F9, 0x0206, 0x0213, 0x0221,
    0x022F, 0x023D, 0x024B, 0x0259, 0x0268, 0x0276, 0x0285, 0x0294, 0x02A3, 0x02B3, 0x02C2, 0x02D2, 0x02E2, 0x02F2, 0x0302, 0x0312, 0x0323, 0x0333, 0x0344, 0x0355,
    0x0366, 0x0378, 0x0389, 0x039B, 0x03AD, 0x03BF, 0x03D1, 0x03E3, 0x03F6, 0x0408, 0x041B, 0x042E, 0x0441, 0x0455, 0x0468, 0x047C, 0x0490, 0x04A4, 0x04B8, 0x04CC,
    0x04E0, 0x04F5, 0x050A, 0x051F, 0x0534, 0x0549, 0x055F, 0x0574, 0x058A, 0x05A0, 0x05B6, 0x05CC, 0x05E2, 0x05F9, 0x0610, 0x0627, 0x063E, 0x0655, 0x066C, 0x0684,
    0x069B, 0x06B3, 0x06CB, 0x06E3, 0x06FC, 0x0714, 0x072D, 0x0745, 0x075E, 0x0777, 0x0791, 0x07AA, 0x07C3, 0x07DD, 0x07F7, 0x0811, 0x082B, 0x0845, 0x0860, 0x087A,
    0x0895, 0x08B0, 0x08CB, 0x08E6, 0x0902, 0x091D, 0x0939, 0x0955, 0x0971, 0x098D, 0x09A9, 0x09C6, 0x09E2, 0x09FF, 0x0A1C, 0x0A39, 0x0A56, 0x0A73, 0x0A91, 0x0AAE,
    0x0ACC, 0x0AEA, 0x0B08, 0x0B26, 0x0B44, 0x0B63, 0x0B81, 0x0BA0, 0x0BBF, 0x0BDE, 0x0BFD, 0x0C1D, 0x0C3C, 0x0C5C, 0x0C7B, 0x0C9B, 0x0CBB, 0x0CDC, 0x0CFC, 0x0D1C,
    0x0D3D, 0x0D5E, 0x0D7F, 0x0DA0, 0x0DC1, 0x0DE2, 0x0E04, 0x0E25, 0x0E47, 0x0E69, 0x0E8B, 0x0EAD, 0x0ECF, 0x0EF1, 0x0F14, 0x0F37, 0x0F59, 0x0F7C, 0x0F9F, 0x0FC2,
    0x0FE6, 0x1009, 0x102D, 0x1051, 0x1074, 0x1098, 0x10BC, 0x10E1, 0x1105, 0x112A, 0x114E, 0x1173, 0x1198, 0x11BD, 0x11E2, 0x1207, 0x122D, 0x1252, 0x1278, 0x129D,
    0x12C3, 0x12E9, 0x130F, 0x1336, 0x135C, 0x1383, 0x13A9, 0x13D0, 0x13F7, 0x141E, 0x1445, 0x146C, 0x1494, 0x14BB, 0x14E3, 0x150A, 0x1532, 0x155A, 0x1582, 0x15AA,
    0x15D3, 0x15FB, 0x1623, 0x164C, 0x1675, 0x169E, 0x16C7, 0x16F0, 0x1719, 0x1742, 0x176C, 0x1795, 0x17BF, 0x17E9, 0x1813, 0x183D, 0x1867, 0x1891, 0x18BB, 0x18E6,
    0x1910, 0x193B, 0x1965, 0x1990, 0x19BB, 0x19E6, 0x1A11, 0x1A3D, 0x1A68, 0x1A93, 0x1ABF, 0x1AEB, 0x1B17, 0x1B42, 0x1B6E, 0x1B9A, 0x1BC7, 0x1BF3, 0x1C1F, 0x1C4C,
    0x1C78, 0x1CA5, 0x1CD2, 0x1CFF, 0x1D2C, 0x1D59, 0x1D86, 0x1DB3, 0x1DE0, 0x1E0E, 0x1E3B, 0x1E69, 0x1E97, 0x1EC4, 0x1EF2, 0x1F20, 0x1F4E, 0x1F7C, 0x1FAB, 0x1FD9,
    0x2007, 0x2036, 0x2065, 0x2093, 0x20C2, 0x20F1, 0x2120, 0x214F, 0x217E, 0x21AD, 0x21DD, 0x220C, 0x223B, 0x226B, 0x229A, 0x22CA, 0x22FA, 0x232A, 0x235A, 0x238A,
    0x23BA, 0x23EA, 0x241A, 0x244B, 0x247B, 0x24AB, 0x24DC, 0x250D, 0x253D, 0x256E, 0x259F, 0x25D0, 0x2601, 0x2632, 0x2663, 0x2694, 0x26C5, 0x26F7, 0x2728, 0x275A,
    0x278B, 0x27BD, 0x27EE, 0x2820, 0x2852, 0x2884, 0x28B6, 0x28E8, 0x291A, 0x294C, 0x297E, 0x29B0, 0x29E3, 0x2A15, 0x2A47, 0x2A7A, 0x2AAC, 0x2ADF, 0x2B12, 0x2B44,
    0x2B77, 0x2BAA, 0x2BDD, 0x2C10, 0x2C43, 0x2C76, 0x2CA9, 0x2CDC, 0x2D0F, 0x2D43, 0x2D76, 0x2DA9, 0x2DDD, 0x2E10, 0x2E44, 0x2E77, 0x2EAB, 0x2EDF, 0x2F12, 0x2F46,
    0x2F7A, 0x2FAE, 0x2FE2, 0x3016, 0x304A, 0x307E, 0x30B2, 0x30E6, 0x311A, 0x314E, 0x3182, 0x31B7, 0x31EB, 0x321F, 0x3254, 0x3288, 0x32BD, 0x32F1, 0x3326, 0x335A,
    0x338F, 0x33C3, 0x33F8, 0x342D, 0x3461, 0x3496, 0x34CB, 0x3500, 0x3535, 0x356A, 0x359F, 0x35D4, 0x3608, 0x363D, 0x3673, 0x36A8, 0x36DD, 0x3712, 0x3747, 0x377C,
    0x37B1, 0x37E6, 0x381C, 0x3851, 0x3886, 0x38BB, 0x38F1, 0x3926, 0x395B, 0x3991, 0x39C6, 0x39FC, 0x3A31, 0x3A66, 0x3A9C, 0x3AD1, 0x3B07, 0x3B3C, 0x3B72, 0x3BA7,
    0x3BDD, 0x3C12, 0x3C48, 0x3C7D, 0x3CB3, 0x3CE9, 0x3D1E, 0x3D54, 0x3D89, 0x3DBF, 0x3DF5, 0x3E2A, 0x3E60, 0x3E95, 0x3ECB, 0x3F01, 0x3F36, 0x3F6C, 0x3FA2, 0x3FD7,
    0x400D, 0x4043, 0x4078, 0x40AE, 0x40E3, 0x4119, 0x414F, 0x4184, 0x41BA, 0x41F0, 0x4225, 0x425B, 0x4290, 0x42C6, 0x42FC, 0x4331, 0x4367, 0x439C, 0x43D2, 0x4407,
    0x443D, 0x4472, 0x44A8, 0x44DD, 0x4513, 0x4548, 0x457E, 0x45B3, 0x45E9, 0x461E, 0x4654, 0x4689, 0x46BE, 0x46F4, 0x4729, 0x475E, 0x4793, 0x47C9, 0x47FE, 0x4833,
    0x4868, 0x489E, 0x48D3, 0x4908, 0x493D, 0x4972, 0x49A7, 0x49DC, 0x4A11, 0x4A46, 0x4A7B, 0x4AB0, 0x4AE5, 0x4B1A, 0x4B4E, 0x4B83, 0x4BB8, 0x4BED, 0x4C21, 0x4C56,
    0x4C8B, 0x4CBF, 0x4CF4, 0x4D28, 0x4D5D, 0x4D91, 0x4DC6, 0x4DFA, 0x4E2E, 0x4E63, 0x4E97, 0x4ECB, 0x4EFF, 0x4F33, 0x4F67, 0x4F9B, 0x4FCF, 0x5003, 0x5037, 0x506B,
    0x509F, 0x50D3, 0x5106, 0x513A, 0x516E, 0x51A1, 0x51D5, 0x5208, 0x523C, 0x526F, 0x52A3, 0x52D6, 0x5309, 0x533C, 0x536F, 0x53A3, 0x53D6, 0x5409, 0x543B, 0x546E,
    0x54A1, 0x54D4, 0x5507, 0x5539, 0x556C, 0x559E, 0x55D1, 0x5603, 0x5636, 0x5668, 0x569A, 0x56CC, 0x56FE, 0x5730, 0x5762, 0x5794, 0x57C6, 0x57F8, 0x5829, 0x585B,
    0x588D, 0x58BE, 0x58F0, 0x5921, 0x5952, 0x5984, 0x59B5, 0x59E6, 0x5A17, 0x5A48, 0x5A79, 0x5AA9, 0x5ADA, 0x5B0B, 0x5B3B, 0x5B6C, 0x5B9C, 0x5BCD, 0x5BFD, 0x5C2D,
    0x5C5D, 0x5C8D, 0x5CBD, 0x5CED, 0x5D1D, 0x5D4D, 0x5D7C, 0x5DAC, 0x5DDB, 0x5E0B, 0x5E3A, 0x5E69, 0x5E99, 0x5EC8, 0x5EF7, 0x5F26, 0x5F54, 0x5F83, 0x5FB2, 0x5FE0,
    0x600F, 0x603D, 0x606B, 0x609A, 0x60C8, 0x60F6, 0x6124, 0x6152, 0x617F, 0x61AD, 0x61DB, 0x6208, 0x6235, 0x6263, 0x6290, 0x62BD, 0x62EA, 0x6317, 0x6344, 0x6370,
    0x639D, 0x63CA, 0x63F6, 0x6422, 0x644E, 0x647B, 0x64A7, 0x64D3, 0x64FE, 0x652A, 0x6556, 0x6581, 0x65AD, 0x65D8, 0x6603, 0x662E, 0x6659, 0x6684, 0x66AF, 0x66DA,
    0x6704, 0x672F, 0x6759, 0x6783, 0x67AD, 0x67D7, 0x6801, 0x682B, 0x6855, 0x687E, 0x68A8, 0x68D1, 0x68FB, 0x6924, 0x694D, 0x6976, 0x699F, 0x69C7, 0x69F0, 0x6A18,
    0x6A41, 0x6A69, 0x6A91, 0x6AB9, 0x6AE1, 0x6B09, 0x6B30, 0x6B58, 0x6B7F, 0x6BA6, 0x6BCE, 0x6BF5, 0x6C1C, 0x6C42, 0x6C69, 0x6C90, 0x6CB6, 0x6CDC, 0x6D03, 0x6D29,
    0x6D4F, 0x6D74, 0x6D9A, 0x6DC0, 0x6DE5, 0x6E0A, 0x6E30, 0x6E55, 0x6E7A, 0x6E9E, 0x6EC3, 0x6EE8, 0x6F0C, 0x6F30, 0x6F55, 0x6F79, 0x6F9D, 0x6FC0, 0x6FE4, 0x7008,
    0x702B, 0x704E, 0x7071, 0x7094, 0x70B7, 0x70DA, 0x70FC, 0x711F, 0x7141, 0x7163, 0x7185, 0x71A7, 0x71C9, 0x71EB, 0x720C, 0x722E, 0x724F, 0x7270, 0x7291, 0x72B2,
    0x72D2, 0x72F3, 0x7313, 0x7333, 0x7354, 0x7374, 0x7393, 0x73B3, 0x73D3, 0x73F2, 0x7411, 0x7430, 0x744F, 0x746E, 0x748D, 0x74AB, 0x74CA, 0x74E8, 0x7506, 0x7524,
    0x7542, 0x7560, 0x757D, 0x759B, 0x75B8, 0x75D5, 0x75F2, 0x760F, 0x762B, 0x7648, 0x7664, 0x7680, 0x769C, 0x76B8, 0x76D4, 0x76F0, 0x770B, 0x7726, 0x7741, 0x775C,
    0x7777, 0x7792, 0x77AC, 0x77C7, 0x77E1, 0x77FB, 0x7815, 0x782F, 0x7848, 0x7862, 0x787B, 0x7894, 0x78AD, 0x78C6, 0x78DF, 0x78F7, 0x7910, 0x7928, 0x7940, 0x7958,
    0x7970, 0x7987, 0x799F, 0x79B6, 0x79CD, 0x79E4, 0x79FB, 0x7A11, 0x7A28, 0x7A3E, 0x7A54, 0x7A6A, 0x7A80, 0x7A96, 0x7AAB, 0x7AC1, 0x7AD6, 0x7AEB, 0x7B00, 0x7B14,
    0x7B29, 0x7B3D, 0x7B51, 0x7B65, 0x7B79, 0x7B8D, 0x7BA1, 0x7BB4, 0x7BC7, 0x7BDA, 0x7BED, 0x7C00, 0x7C13, 0x7C25, 0x7C37, 0x7C49, 0x7C5B, 0x7C6D, 0x7C7F, 0x7C90,
    0x7CA1, 0x7CB2, 0x7CC3, 0x7CD4, 0x7CE5, 0x7CF5, 0x7D05, 0x7D15, 0x7D25, 0x7D35, 0x7D45, 0x7D54, 0x7D63, 0x7D72, 0x7D81, 0x7D90, 0x7D9F, 0x7DAD, 0x7DBB, 0x7DC9,
    0x7DD7, 0x7DE5, 0x7DF2, 0x7E00, 0x7E0D, 0x7E1A, 0x7E27, 0x7E34, 0x7E40, 0x7E4C, 0x7E59, 0x7E65, 0x7E71, 0x7E7C, 0x7E88, 0x7E93, 0x7E9E, 0x7EA9, 0x7EB4, 0x7EBF,
    0x7EC9, 0x7ED3, 0x7EDE, 0x7EE7, 0x7EF1, 0x7EFB, 0x7F04, 0x7F0E, 0x7F17, 0x7F20, 0x7F28, 0x7F31, 0x7F39, 0x7F41, 0x7F4A, 0x7F51, 0x7F59, 0x7F61, 0x7F68, 0x7F6F,
    0x7F76, 0x7F7D, 0x7F84, 0x7F8A, 0x7F90, 0x7F97, 0x7F9D, 0x7FA2, 0x7FA8, 0x7FAD, 0x7FB3, 0x7FB8, 0x7FBD, 0x7FC1, 0x7FC6, 0x7FCA, 0x7FCF, 0x7FD3, 0x7FD6, 0x7FDA,
    0x7FDE, 0x7FE1, 0x7FE4, 0x7FE7, 0x7FEA, 0x7FED, 0x7FEF, 0x7FF1, 0x7FF3, 0x7FF5, 0x7FF7, 0x7FF9, 0x7FFA, 0x7FFB, 0x7FFC, 0x7FFD, 0x7FFE, 0x7FFE, 0x7FFF, 0x7FFF
};

#define MIN(a, b) (((a) < (b)) ? (a) : (b))

std::size_t Resampler::Convert48To44(std::int16_t* from, std::int16_t* to, std::size_t fromLen, std::size_t toLen)
{
    std::size_t outLen = fromLen * 147 / 160;
    if (toLen < outLen)
        outLen = toLen;
    for (unsigned int offset = 0; offset < outLen; offset++)
    {
        float offsetf = offset * 160.0f / 147.0f;
        float factor = offsetf - floorf(offsetf);
        to[offset] = static_cast<std::int16_t>(from[static_cast<std::size_t>(floorf(offsetf))] * (1 - factor) + from[static_cast<std::size_t>(ceilf(offsetf))] * factor);
    }
    return outLen;
}

std::size_t Resampler::Convert44To48(std::int16_t* from, std::int16_t* to, std::size_t fromLen, std::size_t toLen)
{
    std::size_t outLen = fromLen * 160 / 147;
    if (toLen < outLen)
        outLen = toLen;
    unsigned int offset;
    for (offset = 0; offset < outLen; offset++)
    {
        float offsetf = offset * 147.0f / 160.0f;
        float factor = offsetf - floorf(offsetf);
        to[offset] = static_cast<std::int16_t>(from[static_cast<std::size_t>(floorf(offsetf))] * (1 - factor) + from[static_cast<std::size_t>(ceilf(offsetf))] * factor);
    }
    return outLen;
}

std::size_t Resampler::Convert(std::int16_t* from, std::int16_t* to, std::size_t fromLen, std::size_t toLen, std::size_t num, std::size_t denom)
{
    std::size_t outLen = fromLen * num / denom;
    if (toLen < outLen)
        outLen = toLen;
    unsigned int offset;
    for (offset = 0; offset < outLen; offset++)
    {
        float offsetf = offset * static_cast<float>(denom) / static_cast<float>(num);
        float factor = offsetf - floorf(offsetf);
        to[offset] = static_cast<std::int16_t>(from[static_cast<std::size_t>(floorf(offsetf))] * (1 - factor) + from[static_cast<std::size_t>(ceilf(offsetf))] * factor);
    }
    return outLen;
}

void Resampler::Rescale60To80(std::int16_t* in, std::int16_t* out)
{
    std::memcpy(out, in, 960 * 2);
    std::memcpy(out + 960 * 3, in + 960 * 2, 960 * 2);
    for (int i = 0; i < 960; i++)
    {
        out[960 + i]  = static_cast<std::int16_t>(((static_cast<std::int32_t>(in[960 + i])       * hann[959 - i]) >> 15) + ((static_cast<std::int32_t>(in[480 + i])       * hann[i]) >> 15));
        out[1920 + i] = static_cast<std::int16_t>(((static_cast<std::int32_t>(in[960 + 480 + i]) * hann[959 - i]) >> 15) + ((static_cast<std::int32_t>(in[480 + 480 + i]) * hann[i]) >> 15));
    }
}

void Resampler::Rescale60To40(std::int16_t* in, std::int16_t* out)
{
    for (int i = 0; i < 960; i++)
    {
        out[i]       = static_cast<std::int16_t>(((static_cast<std::int32_t>(in[i])  * hann[959 - i]) >> 15) + ((static_cast<std::int32_t>(in[480 + i])  * hann[i]) >> 15));
        out[960 + i] = static_cast<std::int16_t>(((static_cast<std::int32_t>(in[1920 + i]) * hann[i]) >> 15) + ((static_cast<std::int32_t>(in[1440 + i]) * hann[959 - i]) >> 15));
    }
}
