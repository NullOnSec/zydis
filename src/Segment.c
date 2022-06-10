/***************************************************************************************************

  Zyan Disassembler Library (Zydis)

  Original Author : Florian Bernd

 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.

***************************************************************************************************/

#include <Zycore/LibC.h>
#include <Zydis/Segment.h>

/* ============================================================================================== */
/* Helpers                                                                                        */
/* ============================================================================================== */

ZYAN_INLINE ZydisInstructionSegmentNoBits ZydisSegmentDecodeNoBits(void)
{
    return (ZydisInstructionSegmentNoBits) {};
}

ZYAN_INLINE ZydisInstructionSegmentREX ZydisSegmentDecodeREX(const ZyanU8 rex)
{
    return (ZydisInstructionSegmentREX)
    {
        .W = 0x01 & (rex >> 3),
        .R = 0x01 & (rex >> 2),
        .X = 0x01 & (rex >> 1),
        .B = 0x01 & (rex >> 0),
    };
}

ZYAN_INLINE ZydisInstructionSegmentVEX ZydisSegmentDecodeVEX(
    const ZyanU8* const vex, const ZyanU8 length)
{
    switch (length)
    {
    case 2:
        return (ZydisInstructionSegmentVEX)
        {
            .R       = 0x01 & (vex[1] >> 7),
            .X       = 1,
            .B       = 1,
            .m_mmmm  = 1,
            .W       = 0,
            .vvvv    = 0x0F & (vex[1] >> 3),
            .L       = 0x01 & (vex[1] >> 2),
            .pp      = 0x03 & (vex[1] >> 0),
        };
    case 3:
        return (ZydisInstructionSegmentVEX)
        {
            .R       = 0x01 & (vex[1] >> 7),
            .X       = 0x01 & (vex[1] >> 6),
            .B       = 0x01 & (vex[1] >> 5),
            .m_mmmm  = 0x1F & (vex[1] >> 0),
            .W       = 0x01 & (vex[2] >> 7),
            .vvvv    = 0x0F & (vex[2] >> 3),
            .L       = 0x01 & (vex[2] >> 2),
            .pp      = 0x03 & (vex[2] >> 0),
        };
    default:
        ZYAN_UNREACHABLE
    }
}

ZYAN_INLINE ZydisInstructionSegmentXOP ZydisSegmentDecodeXOP(const ZyanU8* const xop)
{
    return (ZydisInstructionSegmentXOP)
    {
        .R      = 0x01 & (xop[1] >> 7),
        .X      = 0x01 & (xop[1] >> 6),
        .B      = 0x01 & (xop[1] >> 5),
        .m_mmmm = 0x1F & (xop[1] >> 0),
        .W      = 0x01 & (xop[2] >> 7),
        .vvvv   = 0x0F & (xop[2] >> 3),
        .L      = 0x01 & (xop[2] >> 2),
        .pp     = 0x03 & (xop[2] >> 0),
    };
}

ZYAN_INLINE ZydisInstructionSegmentEVEX ZydisSegmentDecodeEVEX(const ZyanU8* evex)
{
    return (ZydisInstructionSegmentEVEX)
    {
        .R     = 0x01 & (evex[1] >> 7),
        .X     = 0x01 & (evex[1] >> 6),
        .B     = 0x01 & (evex[1] >> 5),
        .R2    = 0x01 & (evex[1] >> 4),
        .mmm   = 0x07 & (evex[1] >> 0),
        .W     = 0x01 & (evex[2] >> 7),
        .vvvv  = 0x0F & (evex[2] >> 3),
        .pp    = 0x03 & (evex[2] >> 0),
        .z     = 0x01 & (evex[3] >> 7),
        .L2    = 0x01 & (evex[3] >> 6),
        .L     = 0x01 & (evex[3] >> 5),
        .b     = 0x01 & (evex[3] >> 4),
        .V2    = 0x01 & (evex[3] >> 3),
        .aaa   = 0x07 & (evex[3] >> 0),
    };
}

ZYAN_INLINE ZydisInstructionSegmentMVEX ZydisSegmentDecodeMVEX(const ZyanU8* mvex)
{
    return (ZydisInstructionSegmentMVEX)
    {
        .R    = 0x01 & (mvex[1] >> 7),
        .X    = 0x01 & (mvex[1] >> 6),
        .B    = 0x01 & (mvex[1] >> 5),
        .R2   = 0x01 & (mvex[1] >> 4),
        .mmmm = 0x0F & (mvex[1] >> 0),
        .W    = 0x01 & (mvex[2] >> 7),
        .vvvv = 0x0F & (mvex[2] >> 3),
        .pp   = 0x03 & (mvex[2] >> 0),
        .E    = 0x01 & (mvex[3] >> 7),
        .SSS  = 0x07 & (mvex[3] >> 4),
        .V2   = 0x01 & (mvex[3] >> 3),
        .kkk  = 0x07 & (mvex[3] >> 0),
    };
}

ZYAN_INLINE ZydisInstructionSegmentModRM ZydisSegmentDecodeModRM(const ZyanU8 modrm)
{
    return (ZydisInstructionSegmentModRM)
    {
        .mod = 0x03 & (modrm >> 6),
        .reg = 0x07 & (modrm >> 3),
        .rm  = 0x07 & (modrm >> 0),
    };
}

ZYAN_INLINE ZydisInstructionSegmentSIB ZydisSegmentDecodeSIB(const ZyanU8 sib)
{
    return (ZydisInstructionSegmentSIB)
    {
        .scale = 0x03 & (sib >> 6),
        .index = 0x07 & (sib >> 3),
        .base  = 0x07 & (sib >> 0),
    };
}

/* ============================================================================================== */
/* Exported functions                                                                             */
/* ============================================================================================== */

ZyanStatus ZydisGetInstructionSegments(const ZydisDecodedInstruction* const instruction,
    const ZyanU8* buffer, ZyanUSize length, ZydisInstructionSegments* const segments)
{
    if (!instruction || !segments || !buffer || (length < instruction->length))
    {
        return ZYAN_STATUS_INVALID_ARGUMENT;
    }

#   define ZYDIS_CHECK_AND_ADVANCE_BUFFER(n)                    \
        do {                                                    \
            if (length < (n)) return ZYDIS_STATUS_NO_MORE_DATA; \
            buffer += n;                                        \
            length -= n;                                        \
        } while (0)

    segments->count = 0;

    // Legacy prefixes and `REX`.
    if (instruction->raw.prefix_count)
    {
        const ZyanU8 rex_offset = (instruction->attributes & ZYDIS_ATTRIB_HAS_REX) ? 1 : 0;
        if (!rex_offset || (instruction->raw.prefix_count > 1))
        {
            ZYDIS_CHECK_AND_ADVANCE_BUFFER(instruction->raw.prefix_count - rex_offset);
            segments->segments[segments->count++] = (ZydisInstructionSegment)
            {
                .offset  = 0,
                .size    = instruction->raw.prefix_count - rex_offset,
                .type    = ZYDIS_INSTR_SEGMENT_PREFIXES,
                .prefix  = ZydisSegmentDecodeNoBits(),
            };
        }
        if (rex_offset)
        {
            ZYDIS_CHECK_AND_ADVANCE_BUFFER(1);
            segments->segments[segments->count++] = (ZydisInstructionSegment)
            {
                .offset = instruction->raw.prefix_count - rex_offset,
                .size   = 1,
                .type   = ZYDIS_INSTR_SEGMENT_REX,
                .rex    = ZydisSegmentDecodeREX(*buffer),
            };
        }
    }

    // Encoding prefixes
    switch (instruction->encoding)
    {
    case ZYDIS_INSTRUCTION_ENCODING_XOP:
        ZYDIS_CHECK_AND_ADVANCE_BUFFER(3);
        segments->segments[segments->count++] = (ZydisInstructionSegment)
        {
            .offset = instruction->raw.xop.offset,
            .size   = 3,
            .type   = ZYDIS_INSTR_SEGMENT_XOP,
            .xop    = ZydisSegmentDecodeXOP(buffer),
        };
        break;
    case ZYDIS_INSTRUCTION_ENCODING_VEX:
        ZYDIS_CHECK_AND_ADVANCE_BUFFER(instruction->raw.vex.size);
        segments->segments[segments->count++] = (ZydisInstructionSegment)
        {
            .offset = instruction->raw.vex.offset,
            .size   = instruction->raw.vex.size,
            .type   = ZYDIS_INSTR_SEGMENT_VEX,
            .vex    = ZydisSegmentDecodeVEX(buffer, instruction->raw.vex.size),
        };
        break;
    case ZYDIS_INSTRUCTION_ENCODING_EVEX:
        ZYDIS_CHECK_AND_ADVANCE_BUFFER(4);
        segments->segments[segments->count++] = (ZydisInstructionSegment)
        {
            .offset = instruction->raw.evex.offset,
            .size   = 4,
            .type   = ZYDIS_INSTR_SEGMENT_EVEX,
            .evex   = ZydisSegmentDecodeEVEX(buffer),
        };
        break;
    case ZYDIS_INSTRUCTION_ENCODING_MVEX:
        ZYDIS_CHECK_AND_ADVANCE_BUFFER(4);
        segments->segments[segments->count++] = (ZydisInstructionSegment)
        {
            .offset = instruction->raw.mvex.offset,
            .size   = 4,
            .type   = ZYDIS_INSTR_SEGMENT_MVEX,
            .mvex   = ZydisSegmentDecodeMVEX(buffer),
        };
        break;
    default:
        break;
    }

    // Opcode
    ZyanU8 opcode_size = 1;
    if ((instruction->encoding == ZYDIS_INSTRUCTION_ENCODING_LEGACY) ||
        (instruction->encoding == ZYDIS_INSTRUCTION_ENCODING_3DNOW))
    {
        switch (instruction->opcode_map)
        {
        case ZYDIS_OPCODE_MAP_DEFAULT:
            break;
        case ZYDIS_OPCODE_MAP_0F:
            ZYAN_FALLTHROUGH;
        case ZYDIS_OPCODE_MAP_0F0F:
            opcode_size = 2;
            break;
        case ZYDIS_OPCODE_MAP_0F38:
            ZYAN_FALLTHROUGH;
        case ZYDIS_OPCODE_MAP_0F3A:
            opcode_size = 3;
            break;
        default:
            ZYAN_UNREACHABLE;
        }
    }

    ZyanU8 opcode_offset = 0;
    if (segments->count)
    {
        const ZydisInstructionSegment * const prev =
            &segments->segments[segments->count - 1];
        opcode_offset = prev->offset + prev->size;
    }

    ZYDIS_CHECK_AND_ADVANCE_BUFFER(opcode_size);
    segments->segments[segments->count++] = (ZydisInstructionSegment)
    {
        .offset = opcode_offset,
        .size   = opcode_size,
        .type   = ZYDIS_INSTR_SEGMENT_OPCODE,
        .opcode = ZydisSegmentDecodeNoBits(),
    };

    // ModRM
    if (instruction->attributes & ZYDIS_ATTRIB_HAS_MODRM)
    {
        ZYDIS_CHECK_AND_ADVANCE_BUFFER(1);
        segments->segments[segments->count++] = (ZydisInstructionSegment)
        {
            .offset = instruction->raw.modrm.offset,
            .size   = 1,
            .type   = ZYDIS_INSTR_SEGMENT_MODRM,
            .modrm  = ZydisSegmentDecodeModRM(*buffer),
        };
    }

    // SIB
    if (instruction->attributes & ZYDIS_ATTRIB_HAS_SIB)
    {
        ZYDIS_CHECK_AND_ADVANCE_BUFFER(1);
        segments->segments[segments->count++] = (ZydisInstructionSegment)
        {
            .offset = instruction->raw.sib.offset,
            .size   = 1,
            .type   = ZYDIS_INSTR_SEGMENT_SIB,
            .sib    = ZydisSegmentDecodeSIB(*buffer),
        };
    }

    // Displacement
    if (instruction->raw.disp.size)
    {
        ZYDIS_CHECK_AND_ADVANCE_BUFFER(instruction->raw.disp.size / 8);
        segments->segments[segments->count++] = (ZydisInstructionSegment)
        {
            .offset = instruction->raw.disp.offset,
            .size   = instruction->raw.disp.size / 8,
            .type   = ZYDIS_INSTR_SEGMENT_DISPLACEMENT,
            .disp   = ZydisSegmentDecodeNoBits(),
        };
    }

    // Immediates
    for (ZyanU8 i = 0; i < 2; ++i)
    {
        if (instruction->raw.imm[i].size)
        {
            ZYDIS_CHECK_AND_ADVANCE_BUFFER(instruction->raw.imm[i].size / 8);
            segments->segments[segments->count++] = (ZydisInstructionSegment)
            {
                .offset = instruction->raw.imm[i].offset,
                .size   = instruction->raw.imm[i].size / 8,
                .type   = ZYDIS_INSTR_SEGMENT_IMMEDIATE,
                .imm    = ZydisSegmentDecodeNoBits(),
            };
        }
    }

    // 3DNow! suffix opcode
    if (instruction->encoding == ZYDIS_INSTRUCTION_ENCODING_3DNOW)
    {
        ZYDIS_CHECK_AND_ADVANCE_BUFFER(1);
        segments->segments[segments->count++] = (ZydisInstructionSegment)
        {
            .offset = instruction->length - 1,
            .size   = 1,
            .type   = ZYDIS_INSTR_SEGMENT_OPCODE,
            .opcode = ZydisSegmentDecodeNoBits(),
        };
    }

    // Zero out remaining entries
    ZYAN_MEMSET(&segments->segments[segments->count], 0,
        sizeof(*segments->segments) - segments->count);

#   undef ZYDIS_CHECK_AND_ADVANCE_BUFFER

    return ZYAN_STATUS_SUCCESS;
}

const ZydisInstructionSegmentReflectionInfo* ZydisSegmentGetReflectionInfo(
    ZydisInstructionSegmentKind kind, ZyanU8 length)
{
#   define ZYDIS_DEF_OFFS(name, width, offs, struc)                             \
        {                                                                       \
            .field_name = #name,                                                \
            .bit_width = (width),                                               \
            .value_source.bit_offset = (offs),                                  \
            .struct_offset = offsetof(ZydisInstructionSegment##struc, name)     \
        }
#   define ZYDIS_DEF_CNST(name, c, struc)                                       \
        {                                                                       \
            .field_name = #name,                                                \
            .bit_width = 0,                                                     \
            .value_source.constant = (c),                                       \
            .struct_offset = offsetof(ZydisInstructionSegment##struc, name)     \
        }

    static const ZydisInstructionSegmentReflectionInfo rex[] =
    {
        ZYDIS_DEF_OFFS(W, 1, 3, REX),
        ZYDIS_DEF_OFFS(R, 1, 2, REX),
        ZYDIS_DEF_OFFS(X, 1, 1, REX),
        ZYDIS_DEF_OFFS(B, 1, 0, REX),
        {}, // terminating empty record
    };

    static const ZydisInstructionSegmentReflectionInfo vex2[] =
    {
        ZYDIS_DEF_OFFS(R,      1, 15, VEX),
        ZYDIS_DEF_CNST(X,      1,     VEX),
        ZYDIS_DEF_CNST(B,      1,     VEX),
        ZYDIS_DEF_CNST(m_mmmm, 1,     VEX),
        ZYDIS_DEF_CNST(W,      0,     VEX),
        ZYDIS_DEF_OFFS(vvvv,   4, 11, VEX),
        ZYDIS_DEF_OFFS(L,      1, 10, VEX),
        ZYDIS_DEF_OFFS(pp,     2, 8,  VEX),
        {},
    };

    static const ZydisInstructionSegmentReflectionInfo vex3[] =
    {
        ZYDIS_DEF_OFFS(R,      15, 1, VEX),
        ZYDIS_DEF_OFFS(X,      14, 1, VEX),
        ZYDIS_DEF_OFFS(B,      13, 1, VEX),
        ZYDIS_DEF_OFFS(m_mmmm,  8, 5, VEX),
        ZYDIS_DEF_OFFS(W,      23, 1, VEX),
        ZYDIS_DEF_OFFS(vvvv,   19, 4, VEX),
        ZYDIS_DEF_OFFS(L,      18, 1, VEX),
        ZYDIS_DEF_OFFS(pp,     16, 2, VEX),
        {},
    };

    static const ZydisInstructionSegmentReflectionInfo xop[] =
    {
        ZYDIS_DEF_OFFS(R,      15, 1, XOP),
        ZYDIS_DEF_OFFS(X,      14, 1, XOP),
        ZYDIS_DEF_OFFS(B,      13, 1, XOP),
        ZYDIS_DEF_OFFS(m_mmmm,  8, 5, XOP),
        ZYDIS_DEF_OFFS(W,      23, 1, XOP),
        ZYDIS_DEF_OFFS(vvvv,   19, 4, XOP),
        ZYDIS_DEF_OFFS(L,      18, 1, XOP),
        ZYDIS_DEF_OFFS(pp,     16, 2, XOP),
        {},
    };

    static const ZydisInstructionSegmentReflectionInfo evex[] =
    {
        ZYDIS_DEF_OFFS(R,    15, 1, EVEX),
        ZYDIS_DEF_OFFS(X,    14, 1, EVEX),
        ZYDIS_DEF_OFFS(B,    13, 1, EVEX),
        ZYDIS_DEF_OFFS(R2,   12, 1, EVEX),
        ZYDIS_DEF_OFFS(mmm,   8, 3, EVEX),
        ZYDIS_DEF_OFFS(W,    23, 1, EVEX),
        ZYDIS_DEF_OFFS(vvvv, 19, 4, EVEX),
        ZYDIS_DEF_OFFS(pp,   16, 2, EVEX),
        ZYDIS_DEF_OFFS(z,    31, 1, EVEX),
        ZYDIS_DEF_OFFS(L2,   30, 1, EVEX),
        ZYDIS_DEF_OFFS(L,    29, 1, EVEX),
        ZYDIS_DEF_OFFS(b,    28, 1, EVEX),
        ZYDIS_DEF_OFFS(V2,   27, 1, EVEX),
        ZYDIS_DEF_OFFS(aaa,  24, 3, EVEX),
        {},
    };

    static const ZydisInstructionSegmentReflectionInfo mvex[] =
    {
        ZYDIS_DEF_OFFS(R,    15, 1, MVEX),
        ZYDIS_DEF_OFFS(X,    14, 1, MVEX),
        ZYDIS_DEF_OFFS(B,    13, 1, MVEX),
        ZYDIS_DEF_OFFS(R2,   12, 1, MVEX),
        ZYDIS_DEF_OFFS(mmmm,  8, 4, MVEX),
        ZYDIS_DEF_OFFS(W,    23, 1, MVEX),
        ZYDIS_DEF_OFFS(vvvv, 19, 4, MVEX),
        ZYDIS_DEF_OFFS(pp,   16, 2, MVEX),
        ZYDIS_DEF_OFFS(E,    31, 1, MVEX),
        ZYDIS_DEF_OFFS(SSS,  28, 3, MVEX),
        ZYDIS_DEF_OFFS(V2,   27, 1, MVEX),
        ZYDIS_DEF_OFFS(kkk,  24, 3, MVEX),
        {},
    };

    static const ZydisInstructionSegmentReflectionInfo modrm[] =
    {
        ZYDIS_DEF_OFFS(mod, 6, 2, ModRM),
        ZYDIS_DEF_OFFS(reg, 3, 3, ModRM),
        ZYDIS_DEF_OFFS(rm,  0, 3, ModRM),
        {},
    };

    static const ZydisInstructionSegmentReflectionInfo sib[] =
    {
        ZYDIS_DEF_OFFS(scale, 6, 2, SIB),
        ZYDIS_DEF_OFFS(index, 3, 3, SIB),
        ZYDIS_DEF_OFFS(base,  0, 3, SIB),
        {},
    };

    static const ZydisInstructionSegmentReflectionInfo nobits[] = {{}};

    switch (kind)
    {
    case ZYDIS_INSTR_SEGMENT_OPCODE:        return nobits;
    case ZYDIS_INSTR_SEGMENT_MODRM:         return modrm;
    case ZYDIS_INSTR_SEGMENT_REX:           return rex;
    case ZYDIS_INSTR_SEGMENT_DISPLACEMENT:  return nobits;
    case ZYDIS_INSTR_SEGMENT_PREFIXES:      return nobits;
    case ZYDIS_INSTR_SEGMENT_XOP:           return xop;
    case ZYDIS_INSTR_SEGMENT_EVEX:          return evex;
    case ZYDIS_INSTR_SEGMENT_MVEX:          return mvex;
    case ZYDIS_INSTR_SEGMENT_SIB:           return sib;
    case ZYDIS_INSTR_SEGMENT_IMMEDIATE:     return nobits;
    case ZYDIS_INSTR_SEGMENT_VEX:
        return length == 2 ? vex2 :
               length == 3 ? vex3 :
               ZYAN_NULL;
    default:
        return ZYAN_NULL;
    }

#   undef ZYDIS_DEF_OFFS
#   undef ZYDIS_DEF_CONST
}

const char* ZydisSegmentKindGetString(ZydisInstructionSegmentKind kind)
{
    static const char* segments[ZYDIS_INSTR_SEGMENT_MAX_VALUE + 1] = {
        [ZYDIS_INSTR_SEGMENT_PREFIXES       ] = "prefixes",
        [ZYDIS_INSTR_SEGMENT_REX            ] = "rex",
        [ZYDIS_INSTR_SEGMENT_XOP            ] = "xop",
        [ZYDIS_INSTR_SEGMENT_VEX            ] = "vex",
        [ZYDIS_INSTR_SEGMENT_EVEX           ] = "evex",
        [ZYDIS_INSTR_SEGMENT_MVEX           ] = "mvex",
        [ZYDIS_INSTR_SEGMENT_OPCODE         ] = "opcode",
        [ZYDIS_INSTR_SEGMENT_MODRM          ] = "modrm",
        [ZYDIS_INSTR_SEGMENT_SIB            ] = "sib",
        [ZYDIS_INSTR_SEGMENT_DISPLACEMENT   ] = "displacement",
        [ZYDIS_INSTR_SEGMENT_IMMEDIATE      ] = "immediate",
    };

    if (kind > ZYDIS_INSTR_SEGMENT_MAX_VALUE)
    {
        return ZYAN_NULL;
    }

    const char* str = segments[kind];
    ZYAN_ASSERT(str);
    return str;
}

/* ============================================================================================== */
