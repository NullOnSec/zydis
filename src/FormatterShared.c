/***************************************************************************************************

  Zyan Disassembler Library (Zydis)

  Original Author : Florian Bernd, Joel Hoener

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

#include <Zydis/Internal/FormatterShared.h>
#include <Zydis/Utils.h>

/* ============================================================================================== */
/* Constants                                                                                      */
/* ============================================================================================== */

/* ---------------------------------------------------------------------------------------------- */
/* String constants                                                                               */
/* ---------------------------------------------------------------------------------------------- */

static const ZydisShortString STR_DELIM_MNEMONIC = ZYDIS_MAKE_SHORTSTRING(" ");
static const ZydisShortString STR_DELIM_SGMENT   = ZYDIS_MAKE_SHORTSTRING(":");
static const ZydisShortString STR_PREF_XACQUIRE  = ZYDIS_MAKE_SHORTSTRING("xacquire ");
static const ZydisShortString STR_PREF_XRELEASE  = ZYDIS_MAKE_SHORTSTRING("xrelease ");
static const ZydisShortString STR_PREF_LOCK      = ZYDIS_MAKE_SHORTSTRING("lock ");
static const ZydisShortString STR_PREF_REP       = ZYDIS_MAKE_SHORTSTRING("rep ");
static const ZydisShortString STR_PREF_REPE      = ZYDIS_MAKE_SHORTSTRING("repe ");
static const ZydisShortString STR_PREF_REPNE     = ZYDIS_MAKE_SHORTSTRING("repne ");
static const ZydisShortString STR_PREF_BND       = ZYDIS_MAKE_SHORTSTRING("bnd ");
static const ZydisShortString STR_PREF_SEG_CS    = ZYDIS_MAKE_SHORTSTRING("cs ");
static const ZydisShortString STR_PREF_SEG_SS    = ZYDIS_MAKE_SHORTSTRING("ss ");
static const ZydisShortString STR_PREF_SEG_DS    = ZYDIS_MAKE_SHORTSTRING("ds ");
static const ZydisShortString STR_PREF_SEG_ES    = ZYDIS_MAKE_SHORTSTRING("es ");
static const ZydisShortString STR_PREF_SEG_FS    = ZYDIS_MAKE_SHORTSTRING("fs ");
static const ZydisShortString STR_PREF_SEG_GS    = ZYDIS_MAKE_SHORTSTRING("gs ");
static const ZydisShortString STR_DECO_BEGIN     = ZYDIS_MAKE_SHORTSTRING(" {");
static const ZydisShortString STR_DECO_END       = ZYDIS_MAKE_SHORTSTRING("}");
static const ZydisShortString STR_DECO_ZERO      = ZYDIS_MAKE_SHORTSTRING(" {z}");
static const ZydisShortString STR_DECO_1TO2      = ZYDIS_MAKE_SHORTSTRING(" {1to2}");
static const ZydisShortString STR_DECO_1TO4      = ZYDIS_MAKE_SHORTSTRING(" {1to4}");
static const ZydisShortString STR_DECO_1TO8      = ZYDIS_MAKE_SHORTSTRING(" {1to8}");
static const ZydisShortString STR_DECO_1TO16     = ZYDIS_MAKE_SHORTSTRING(" {1to16}");
static const ZydisShortString STR_DECO_4TO8      = ZYDIS_MAKE_SHORTSTRING(" {4to8}");
static const ZydisShortString STR_DECO_4TO16     = ZYDIS_MAKE_SHORTSTRING(" {4to16}");
static const ZydisShortString STR_DECO_RN_SAE    = ZYDIS_MAKE_SHORTSTRING(" {rn-sae}");
static const ZydisShortString STR_DECO_RD_SAE    = ZYDIS_MAKE_SHORTSTRING(" {rd-sae}");
static const ZydisShortString STR_DECO_RU_SAE    = ZYDIS_MAKE_SHORTSTRING(" {ru-sae}");
static const ZydisShortString STR_DECO_RZ_SAE    = ZYDIS_MAKE_SHORTSTRING(" {rz-sae}");
static const ZydisShortString STR_DECO_RN        = ZYDIS_MAKE_SHORTSTRING(" {rn}");
static const ZydisShortString STR_DECO_RD        = ZYDIS_MAKE_SHORTSTRING(" {rd}");
static const ZydisShortString STR_DECO_RU        = ZYDIS_MAKE_SHORTSTRING(" {ru}");
static const ZydisShortString STR_DECO_RZ        = ZYDIS_MAKE_SHORTSTRING(" {rz}");
static const ZydisShortString STR_DECO_SAE       = ZYDIS_MAKE_SHORTSTRING(" {sae}");
static const ZydisShortString STR_DECO_CDAB      = ZYDIS_MAKE_SHORTSTRING(" {cdab}");
static const ZydisShortString STR_DECO_BADC      = ZYDIS_MAKE_SHORTSTRING(" {badc}");
static const ZydisShortString STR_DECO_DACB      = ZYDIS_MAKE_SHORTSTRING(" {dacb}");
static const ZydisShortString STR_DECO_AAAA      = ZYDIS_MAKE_SHORTSTRING(" {aaaa}");
static const ZydisShortString STR_DECO_BBBB      = ZYDIS_MAKE_SHORTSTRING(" {bbbb}");
static const ZydisShortString STR_DECO_CCCC      = ZYDIS_MAKE_SHORTSTRING(" {cccc}");
static const ZydisShortString STR_DECO_DDDD      = ZYDIS_MAKE_SHORTSTRING(" {dddd}");
static const ZydisShortString STR_DECO_FLOAT16   = ZYDIS_MAKE_SHORTSTRING(" {float16}");
static const ZydisShortString STR_DECO_SINT8     = ZYDIS_MAKE_SHORTSTRING(" {sint8}");
static const ZydisShortString STR_DECO_UINT8     = ZYDIS_MAKE_SHORTSTRING(" {uint8}");
static const ZydisShortString STR_DECO_SINT16    = ZYDIS_MAKE_SHORTSTRING(" {sint16}");
static const ZydisShortString STR_DECO_UINT16    = ZYDIS_MAKE_SHORTSTRING(" {uint16}");
static const ZydisShortString STR_DECO_EH        = ZYDIS_MAKE_SHORTSTRING(" {eh}");
static const ZydisShortString STR_ADD            = ZYDIS_MAKE_SHORTSTRING("+");

/* ---------------------------------------------------------------------------------------------- */

static const ZydisShortString STR_PREF_REX[0x10] =
{
    /* 40 */ ZYDIS_MAKE_SHORTSTRING("rex "),
    /* 41 */ ZYDIS_MAKE_SHORTSTRING("rex.b "),
    /* 42 */ ZYDIS_MAKE_SHORTSTRING("rex.x "),
    /* 43 */ ZYDIS_MAKE_SHORTSTRING("rex.xb "),
    /* 44 */ ZYDIS_MAKE_SHORTSTRING("rex.r "),
    /* 45 */ ZYDIS_MAKE_SHORTSTRING("rex.rb "),
    /* 46 */ ZYDIS_MAKE_SHORTSTRING("rex.rx "),
    /* 47 */ ZYDIS_MAKE_SHORTSTRING("rex.rxb "),
    /* 48 */ ZYDIS_MAKE_SHORTSTRING("rex.w "),
    /* 49 */ ZYDIS_MAKE_SHORTSTRING("rex.wb "),
    /* 4A */ ZYDIS_MAKE_SHORTSTRING("rex.wx "),
    /* 4B */ ZYDIS_MAKE_SHORTSTRING("rex.wxb "),
    /* 4C */ ZYDIS_MAKE_SHORTSTRING("rex.wr "),
    /* 4D */ ZYDIS_MAKE_SHORTSTRING("rex.wrb "),
    /* 4E */ ZYDIS_MAKE_SHORTSTRING("rex.wrx "),
    /* 4F */ ZYDIS_MAKE_SHORTSTRING("rex.wrxb ")
};

/* ---------------------------------------------------------------------------------------------- */

/* ============================================================================================== */
/* Formatter functions                                                                            */
/* ============================================================================================== */

/* ---------------------------------------------------------------------------------------------- */
/* Operands                                                                                       */
/* ---------------------------------------------------------------------------------------------- */

ZyanStatus ZydisFormatterSharedFormatOperandREG(const ZydisFormatter* formatter,
    ZyanString* string, ZydisFormatterContext* context)
{
    if (!context)
    {
        return ZYAN_STATUS_INVALID_ARGUMENT;
    }

    return formatter->func_print_register(formatter, string, context, context->operand->reg.value);
}

ZyanStatus ZydisFormatterSharedFormatOperandPTR(const ZydisFormatter* formatter,
    ZyanString* string, ZydisFormatterContext* context)
{
    if (!formatter || !context)
    {
        return ZYAN_STATUS_INVALID_ARGUMENT;
    }

    ZYDIS_STRING_APPEND_NUM_U(formatter, formatter->addr_base, string,
        context->operand->ptr.segment, 4);
    ZYAN_CHECK(ZydisStringAppendShort(string, &STR_DELIM_SGMENT));
    ZYDIS_STRING_APPEND_NUM_U(formatter, formatter->addr_base, string,
        context->operand->ptr.offset , 8);

    return ZYAN_STATUS_SUCCESS;
}

ZyanStatus ZydisFormatterSharedFormatOperandIMM(const ZydisFormatter* formatter,
    ZyanString* string, ZydisFormatterContext* context)
{
    if (!formatter || !context)
    {
        return ZYAN_STATUS_INVALID_ARGUMENT;
    }

    // The immediate operand contains an address
    if (context->operand->imm.is_relative)
    {
        const ZyanBool absolute = (context->runtime_address != ZYDIS_RUNTIME_ADDRESS_NONE);
        if (absolute)
        {
            return formatter->func_print_address_abs(formatter, string, context);
        }
        return formatter->func_print_address_rel(formatter, string, context);
    }

    // The immediate operand contains an actual ordinal value
    return formatter->func_print_imm(formatter, string, context);
}

/* ---------------------------------------------------------------------------------------------- */
/* Elemental tokens                                                                               */
/* ---------------------------------------------------------------------------------------------- */

ZyanStatus ZydisFormatterSharedPrintAddressABS(const ZydisFormatter* formatter,
    ZyanString* string, ZydisFormatterContext* context)
{
    if (!formatter || !context)
    {
        return ZYAN_STATUS_INVALID_ARGUMENT;
    }

    ZyanU64 address;
    ZYAN_CHECK(ZydisCalcAbsoluteAddress(context->instruction, context->operand,
        context->runtime_address, &address));
    ZyanU8 padding = (formatter->addr_padding_absolute ==
        ZYDIS_PADDING_AUTO) ? 0 : (ZyanU8)formatter->addr_padding_absolute;
    if ((formatter->addr_padding_absolute == ZYDIS_PADDING_AUTO) &&
        (formatter->addr_base == ZYDIS_NUMERIC_BASE_HEX))
    {
        switch (context->instruction->stack_width)
        {
        case 16:
            padding =  4;
            address = (ZyanU16)address;
            break;
        case 32:
            padding =  8;
            address = (ZyanU32)address;
            break;
        case 64:
            padding = 16;
            break;
        default:
            return ZYAN_STATUS_INVALID_ARGUMENT;
        }
    }
    ZYDIS_STRING_APPEND_NUM_U(formatter, formatter->addr_base, string, address, padding);

    return ZYAN_STATUS_SUCCESS;
}

ZyanStatus ZydisFormatterSharedPrintAddressREL(const ZydisFormatter* formatter,
    ZyanString* string, ZydisFormatterContext* context)
{
    ZyanU64 address;
    ZYAN_CHECK(ZydisCalcAbsoluteAddress(context->instruction, context->operand, 0, &address));

    ZyanU8 padding = (formatter->addr_padding_relative ==
        ZYDIS_PADDING_AUTO) ? 0 : (ZyanU8)formatter->addr_padding_relative;
    if ((formatter->addr_padding_relative == ZYDIS_PADDING_AUTO) &&
        (formatter->addr_base == ZYDIS_NUMERIC_BASE_HEX))
    {
        switch (context->instruction->stack_width)
        {
        case 16:
            padding =  4;
            address = (ZyanU16)address;
            break;
        case 32:
            padding =  8;
            address = (ZyanU32)address;
            break;
        case 64:
            padding = 16;
            break;
        default:
            return ZYAN_STATUS_INVALID_ARGUMENT;
        }
    }

    switch (formatter->addr_signedness)
    {
    case ZYDIS_SIGNEDNESS_AUTO:
    case ZYDIS_SIGNEDNESS_SIGNED:
        ZYDIS_STRING_APPEND_NUM_S(formatter, formatter->addr_base, string, address, padding,
            ZYAN_TRUE);
        break;
    case ZYDIS_SIGNEDNESS_UNSIGNED:
        ZYAN_CHECK(ZydisStringAppendShort(string, &STR_ADD));
        ZYDIS_STRING_APPEND_NUM_U(formatter, formatter->addr_base, string, address, padding);
        break;
    default:
        return ZYAN_STATUS_INVALID_ARGUMENT;
    }

    return ZYAN_STATUS_SUCCESS;
    /*if (!formatter || !context)
    {
        return ZYAN_STATUS_INVALID_ARGUMENT;
    }

    ZyanU8 padding = (formatter->addr_padding_relative == ZYDIS_PADDING_AUTO) ?
        0 : (ZyanU8)formatter->addr_padding_relative;
    if ((formatter->addr_padding_relative == ZYDIS_PADDING_AUTO) &&
        (formatter->addr_base == ZYDIS_NUMERIC_BASE_HEX))
    {
        switch (context->instruction->stack_width)
        {
        case 16: padding =  4; break;
        case 32: padding =  8; break;
        case 64: padding = 16; break;
        default:
            return ZYAN_STATUS_INVALID_ARGUMENT;
        }
    }
    switch (context->operand->type)
    {
    case ZYDIS_OPERAND_TYPE_MEMORY:
        switch (formatter->addr_signedness)
        {
        case ZYDIS_SIGNEDNESS_AUTO:
        case ZYDIS_SIGNEDNESS_SIGNED:
            ZYDIS_STRING_APPEND_NUM_S(formatter, formatter->addr_base, string,
                context->operand->mem.disp.value, padding, ZYAN_TRUE);
            break;
        case ZYDIS_SIGNEDNESS_UNSIGNED:
            ZYDIS_STRING_APPEND_NUM_U(formatter, formatter->addr_base, string,
                context->operand->mem.disp.value, padding);
            break;
        default:
            return ZYAN_STATUS_INVALID_ARGUMENT;
        }
        return ZYAN_STATUS_SUCCESS;
    case ZYDIS_OPERAND_TYPE_IMMEDIATE:
    {
        switch (formatter->addr_signedness)
        {
        case ZYDIS_SIGNEDNESS_AUTO:
        case ZYDIS_SIGNEDNESS_SIGNED:
            ZYDIS_STRING_APPEND_NUM_S(formatter, formatter->addr_base, string,
                context->operand->imm.value.s, padding, ZYAN_TRUE);
            break;
        case ZYDIS_SIGNEDNESS_UNSIGNED:
            ZYDIS_STRING_APPEND_NUM_U(formatter, formatter->addr_base, string,
                context->operand->imm.value.u, padding);
            break;
        default:
            return ZYAN_STATUS_INVALID_ARGUMENT;
        }
        return ZYAN_STATUS_SUCCESS;
    }
    default:
        return ZYAN_STATUS_INVALID_ARGUMENT;
    }*/
}

ZyanStatus ZydisFormatterSharedPrintIMM(const ZydisFormatter* formatter,
    ZyanString* string, ZydisFormatterContext* context)
{
    if (!formatter || !context)
    {
        return ZYAN_STATUS_INVALID_ARGUMENT;
    }

    const ZyanBool is_signed =
        (formatter->imm_signedness == ZYDIS_SIGNEDNESS_SIGNED) ||
        (formatter->imm_signedness == ZYDIS_SIGNEDNESS_AUTO && (context->operand->imm.is_signed));
    if (is_signed && (context->operand->imm.value.s < 0))
    {
        ZYDIS_STRING_APPEND_NUM_S(formatter, formatter->imm_base, string,
            context->operand->imm.value.s, formatter->imm_padding, ZYAN_FALSE);
        return ZYAN_STATUS_SUCCESS;
    }
    ZyanU64 value;
    ZyanU8 padding = (formatter->imm_padding ==
        ZYDIS_PADDING_AUTO) ? 0 : (ZyanU8)formatter->imm_padding;
    switch (context->instruction->operand_width)
    {
    case 8:
        if (formatter->imm_padding == ZYDIS_PADDING_AUTO)
        {
            padding =  2;
        }
        value = (ZyanU8 )context->operand->imm.value.u;
        break;
    case 16:
        if (formatter->imm_padding == ZYDIS_PADDING_AUTO)
        {
            padding =  4;
        }
        value = (ZyanU16)context->operand->imm.value.u;
        break;
    case 32:
        if (formatter->imm_padding == ZYDIS_PADDING_AUTO)
        {
            padding =  8;
        }
        value = (ZyanU32)context->operand->imm.value.u;
        break;
    case 64:
        if (formatter->imm_padding == ZYDIS_PADDING_AUTO)
        {
            padding = 16;
        }
        value = (ZyanU64)context->operand->imm.value.u;
        break;
    default:
        return ZYAN_STATUS_INVALID_ARGUMENT;
    }
    ZYDIS_STRING_APPEND_NUM_U(formatter, formatter->imm_base, string, value, padding);

    return ZYAN_STATUS_SUCCESS;
}

/* ---------------------------------------------------------------------------------------------- */
/* Optional tokens                                                                                */
/* ---------------------------------------------------------------------------------------------- */

ZyanStatus ZydisFormatterSharedPrintSegment(const ZydisFormatter* formatter,
    ZyanString* string, ZydisFormatterContext* context)
{
    if (!formatter || !context)
    {
        return ZYAN_STATUS_INVALID_ARGUMENT;
    }

    ZyanBool printed_segment = ZYAN_FALSE;
    switch (context->operand->mem.segment)
    {
    case ZYDIS_REGISTER_ES:
    case ZYDIS_REGISTER_CS:
    case ZYDIS_REGISTER_FS:
    case ZYDIS_REGISTER_GS:
        ZYAN_CHECK(formatter->func_print_register(formatter, string, context,
            context->operand->mem.segment));
        printed_segment = ZYAN_TRUE;
        break;
    case ZYDIS_REGISTER_SS:
        if ((formatter->force_memory_segment) ||
            (context->instruction->attributes & ZYDIS_ATTRIB_HAS_SEGMENT_SS))
        {
            ZYAN_CHECK(formatter->func_print_register(formatter, string, context,
                context->operand->mem.segment));
            printed_segment = ZYAN_TRUE;
        }
        break;
    case ZYDIS_REGISTER_DS:
        if ((formatter->force_memory_segment) ||
            (context->instruction->attributes & ZYDIS_ATTRIB_HAS_SEGMENT_DS))
        {
            ZYAN_CHECK(formatter->func_print_register(formatter, string, context,
                context->operand->mem.segment));
            printed_segment = ZYAN_TRUE;
        }
        break;
    default:
        break;
    }
    if (printed_segment)
    {
        ZYAN_CHECK(ZydisStringAppendShort(string, &STR_DELIM_SGMENT));
    }

    return ZYAN_STATUS_SUCCESS;
}

ZyanStatus ZydisFormatterSharedPrintPrefixes(const ZydisFormatter* formatter,
    ZyanString* string, ZydisFormatterContext* context)
{
    if (!formatter || !context)
    {
        return ZYAN_STATUS_INVALID_ARGUMENT;
    }

    if (formatter->detailed_prefixes)
    {
        for (ZyanU8 i = 0; i < context->instruction->raw.prefix_count; ++i)
        {
            const ZyanU8 value = context->instruction->raw.prefixes[i].value;
            switch (context->instruction->raw.prefixes[i].type)
            {
            case ZYDIS_PREFIX_TYPE_IGNORED:
            case ZYDIS_PREFIX_TYPE_MANDATORY:
            {
                if ((value & 0xF0) == 0x40)
                {
                    ZYAN_CHECK(ZydisStringAppendShortCase(string, &STR_PREF_REX[value & 0x0F],
                        formatter->letter_case));
                } else
                {
                    switch (value)
                    {
                    case 0xF0:
                        ZYAN_CHECK(ZydisStringAppendShortCase(string, &STR_PREF_LOCK,
                            formatter->letter_case));
                        break;
                    case 0x2E:
                        ZYAN_CHECK(ZydisStringAppendShortCase(string, &STR_PREF_SEG_CS,
                            formatter->letter_case));
                        break;
                    case 0x36:
                        ZYAN_CHECK(ZydisStringAppendShortCase(string, &STR_PREF_SEG_SS,
                            formatter->letter_case));
                        break;
                    case 0x3E:
                        ZYAN_CHECK(ZydisStringAppendShortCase(string, &STR_PREF_SEG_DS,
                            formatter->letter_case));
                        break;
                    case 0x26:
                        ZYAN_CHECK(ZydisStringAppendShortCase(string, &STR_PREF_SEG_ES,
                            formatter->letter_case));
                        break;
                    case 0x64:
                        ZYAN_CHECK(ZydisStringAppendShortCase(string, &STR_PREF_SEG_FS,
                            formatter->letter_case));
                        break;
                    case 0x65:
                        ZYAN_CHECK(ZydisStringAppendShortCase(string, &STR_PREF_SEG_GS,
                            formatter->letter_case));
                        break;
                    default:
                        ZYAN_CHECK(ZydisStringAppendHexU(string, value, 0,
                            formatter->hex_uppercase, ZYAN_NULL, ZYAN_NULL));
                        ZYAN_CHECK(ZydisStringAppendShort(string, &STR_DELIM_MNEMONIC));
                        break;
                    }
                }
                break;
            }
            case ZYDIS_PREFIX_TYPE_EFFECTIVE:
                switch (value)
                {
                case 0xF0:
                    ZYAN_CHECK(ZydisStringAppendShortCase(string, &STR_PREF_LOCK,
                        formatter->letter_case));
                    break;
                case 0xF2:
                    if (context->instruction->attributes & ZYDIS_ATTRIB_HAS_XACQUIRE)
                    {
                        ZYAN_CHECK(ZydisStringAppendShortCase(string, &STR_PREF_XACQUIRE,
                            formatter->letter_case));
                    }
                    if (context->instruction->attributes & ZYDIS_ATTRIB_HAS_REPNE)
                    {
                        ZYAN_CHECK(ZydisStringAppendShortCase(string, &STR_PREF_REPNE,
                            formatter->letter_case));
                    }

                    if (context->instruction->attributes & ZYDIS_ATTRIB_HAS_BND)
                    {
                        ZYAN_CHECK(ZydisStringAppendShortCase(string, &STR_PREF_BND,
                            formatter->letter_case));
                    }
                    break;
                case 0xF3:
                    if (context->instruction->attributes & ZYDIS_ATTRIB_HAS_XRELEASE)
                    {
                        ZYAN_CHECK(ZydisStringAppendShortCase(string, &STR_PREF_XRELEASE,
                            formatter->letter_case));
                    }
                    if (context->instruction->attributes & ZYDIS_ATTRIB_HAS_REP)
                    {
                        ZYAN_CHECK(ZydisStringAppendShortCase(string, &STR_PREF_REP,
                            formatter->letter_case));
                    }
                    if (context->instruction->attributes & ZYDIS_ATTRIB_HAS_REPE)
                    {
                        ZYAN_CHECK(ZydisStringAppendShortCase(string, &STR_PREF_REPE,
                            formatter->letter_case));
                    }
                    break;
                default:
                    break;
                }
                break;
            default:
                return ZYAN_STATUS_INVALID_ARGUMENT;
            }
        }
        return ZYAN_STATUS_SUCCESS;
    }

    if (context->instruction->attributes & ZYDIS_ATTRIB_HAS_XACQUIRE)
    {
        ZYAN_CHECK(ZydisStringAppendShortCase(string, &STR_PREF_XACQUIRE, formatter->letter_case));
    }
    if (context->instruction->attributes & ZYDIS_ATTRIB_HAS_XRELEASE)
    {
        ZYAN_CHECK(ZydisStringAppendShortCase(string, &STR_PREF_XRELEASE, formatter->letter_case));
    }

    if (context->instruction->attributes & ZYDIS_ATTRIB_HAS_LOCK)
    {
        return ZydisStringAppendShortCase(string, &STR_PREF_LOCK, formatter->letter_case);
    }

    if (context->instruction->attributes & ZYDIS_ATTRIB_HAS_REP)
    {
        return ZydisStringAppendShortCase(string, &STR_PREF_REP, formatter->letter_case);
    }
    if (context->instruction->attributes & ZYDIS_ATTRIB_HAS_REPE)
    {
        return ZydisStringAppendShortCase(string, &STR_PREF_REPE, formatter->letter_case);
    }
    if (context->instruction->attributes & ZYDIS_ATTRIB_HAS_REPNE)
    {
        return ZydisStringAppendShortCase(string, &STR_PREF_REPNE, formatter->letter_case);
    }

    if (context->instruction->attributes & ZYDIS_ATTRIB_HAS_BND)
    {
        return ZydisStringAppendShortCase(string, &STR_PREF_BND, formatter->letter_case);
    }

    return ZYAN_STATUS_SUCCESS;
}

ZyanStatus ZydisFormatterSharedPrintDecorator(const ZydisFormatter* formatter,
    ZyanString* string, ZydisFormatterContext* context, ZydisDecorator decorator)
{
    if (!formatter || !context)
    {
        return ZYAN_STATUS_INVALID_ARGUMENT;
    }

    switch (decorator)
    {
    case ZYDIS_DECORATOR_MASK:
    {
#if !defined(ZYDIS_DISABLE_AVX512) || !defined(ZYDIS_DISABLE_KNC)
        if (context->instruction->avx.mask.reg != ZYDIS_REGISTER_K0)
        {
            ZYAN_CHECK(ZydisStringAppendShort(string, &STR_DECO_BEGIN));
            ZYAN_CHECK(formatter->func_print_register(formatter, string, context,
                context->instruction->avx.mask.reg));
            ZYAN_CHECK(ZydisStringAppendShort(string, &STR_DECO_END));

            // Only print the zeroing decorator, if the instruction is not a "zeroing masking only"
            // instruction (e.g. `vcmpsd`)
            if ((context->instruction->avx.mask.mode == ZYDIS_MASK_MODE_ZEROING) &&
                (context->instruction->raw.evex.z))
            {
                ZYAN_CHECK(ZydisStringAppendShort(string, &STR_DECO_ZERO));
            }
        }
#endif
        break;
    }
    case ZYDIS_DECORATOR_BC:
#if !defined(ZYDIS_DISABLE_AVX512)
        if (!context->instruction->avx.broadcast.is_static)
        {
            switch (context->instruction->avx.broadcast.mode)
            {
            case ZYDIS_BROADCAST_MODE_INVALID:
                break;
            case ZYDIS_BROADCAST_MODE_1_TO_2:
                ZYAN_CHECK(ZydisStringAppendShort(string, &STR_DECO_1TO2));
                break;
            case ZYDIS_BROADCAST_MODE_1_TO_4:
                ZYAN_CHECK(ZydisStringAppendShort(string, &STR_DECO_1TO4));
                break;
            case ZYDIS_BROADCAST_MODE_1_TO_8:
                ZYAN_CHECK(ZydisStringAppendShort(string, &STR_DECO_1TO8));
                break;
            case ZYDIS_BROADCAST_MODE_1_TO_16:
                ZYAN_CHECK(ZydisStringAppendShort(string, &STR_DECO_1TO16));
                break;
            case ZYDIS_BROADCAST_MODE_4_TO_8:
                ZYAN_CHECK(ZydisStringAppendShort(string, &STR_DECO_4TO8));
                break;
            case ZYDIS_BROADCAST_MODE_4_TO_16:
                ZYAN_CHECK(ZydisStringAppendShort(string, &STR_DECO_4TO16));
                break;
            default:
                return ZYAN_STATUS_INVALID_ARGUMENT;
            }
        }
#endif
        break;
    case ZYDIS_DECORATOR_RC:
#if !defined(ZYDIS_DISABLE_AVX512)
        if (context->instruction->avx.has_SAE)
        {
            switch (context->instruction->avx.rounding.mode)
            {
            case ZYDIS_ROUNDING_MODE_INVALID:
                break;
            case ZYDIS_ROUNDING_MODE_RN:
                ZYAN_CHECK(ZydisStringAppendShort(string, &STR_DECO_RN_SAE));
                break;
            case ZYDIS_ROUNDING_MODE_RD:
                ZYAN_CHECK(ZydisStringAppendShort(string, &STR_DECO_RD_SAE));
                break;
            case ZYDIS_ROUNDING_MODE_RU:
                ZYAN_CHECK(ZydisStringAppendShort(string, &STR_DECO_RU_SAE));
                break;
            case ZYDIS_ROUNDING_MODE_RZ:
                ZYAN_CHECK(ZydisStringAppendShort(string, &STR_DECO_RZ_SAE));
                break;
            default:
                return ZYAN_STATUS_INVALID_ARGUMENT;
            }
        } else
        {
            switch (context->instruction->avx.rounding.mode)
            {
            case ZYDIS_ROUNDING_MODE_INVALID:
                break;
            case ZYDIS_ROUNDING_MODE_RN:
                ZYAN_CHECK(ZydisStringAppendShort(string, &STR_DECO_RN));
                break;
            case ZYDIS_ROUNDING_MODE_RD:
                ZYAN_CHECK(ZydisStringAppendShort(string, &STR_DECO_RD));
                break;
            case ZYDIS_ROUNDING_MODE_RU:
                ZYAN_CHECK(ZydisStringAppendShort(string, &STR_DECO_RU));
                break;
            case ZYDIS_ROUNDING_MODE_RZ:
                ZYAN_CHECK(ZydisStringAppendShort(string, &STR_DECO_RZ));
                break;
            default:
                return ZYAN_STATUS_INVALID_ARGUMENT;
            }
        }
#endif
        break;
    case ZYDIS_DECORATOR_SAE:
#if !defined(ZYDIS_DISABLE_AVX512)
        if (context->instruction->avx.has_SAE && !context->instruction->avx.rounding.mode)
        {
            ZYAN_CHECK(ZydisStringAppendShort(string, &STR_DECO_SAE));
        }
#endif
        break;
    case ZYDIS_DECORATOR_SWIZZLE:
#if !defined(ZYDIS_DISABLE_KNC)
        switch (context->instruction->avx.swizzle.mode)
        {
        case ZYDIS_SWIZZLE_MODE_INVALID:
        case ZYDIS_SWIZZLE_MODE_DCBA:
            // Nothing to do here
            break;
        case ZYDIS_SWIZZLE_MODE_CDAB:
            ZYAN_CHECK(ZydisStringAppendShort(string, &STR_DECO_CDAB));
            break;
        case ZYDIS_SWIZZLE_MODE_BADC:
            ZYAN_CHECK(ZydisStringAppendShort(string, &STR_DECO_BADC));
            break;
        case ZYDIS_SWIZZLE_MODE_DACB:
            ZYAN_CHECK(ZydisStringAppendShort(string, &STR_DECO_DACB));
            break;
        case ZYDIS_SWIZZLE_MODE_AAAA:
            ZYAN_CHECK(ZydisStringAppendShort(string, &STR_DECO_AAAA));
            break;
        case ZYDIS_SWIZZLE_MODE_BBBB:
            ZYAN_CHECK(ZydisStringAppendShort(string, &STR_DECO_BBBB));
            break;
        case ZYDIS_SWIZZLE_MODE_CCCC:
            ZYAN_CHECK(ZydisStringAppendShort(string, &STR_DECO_CCCC));
            break;
        case ZYDIS_SWIZZLE_MODE_DDDD:
            ZYAN_CHECK(ZydisStringAppendShort(string, &STR_DECO_DDDD));
            break;
        default:
            return ZYAN_STATUS_INVALID_ARGUMENT;
        }
#endif
        break;
    case ZYDIS_DECORATOR_CONVERSION:
#if !defined(ZYDIS_DISABLE_KNC)
        switch (context->instruction->avx.conversion.mode)
        {
        case ZYDIS_CONVERSION_MODE_INVALID:
            break;
        case ZYDIS_CONVERSION_MODE_FLOAT16:
            ZYAN_CHECK(ZydisStringAppendShort(string, &STR_DECO_FLOAT16));
            break;
        case ZYDIS_CONVERSION_MODE_SINT8:
            ZYAN_CHECK(ZydisStringAppendShort(string, &STR_DECO_SINT8));
            break;
        case ZYDIS_CONVERSION_MODE_UINT8:
            ZYAN_CHECK(ZydisStringAppendShort(string, &STR_DECO_UINT8));
            break;
        case ZYDIS_CONVERSION_MODE_SINT16:
            ZYAN_CHECK(ZydisStringAppendShort(string, &STR_DECO_SINT16));
            break;
        case ZYDIS_CONVERSION_MODE_UINT16:
            ZYAN_CHECK(ZydisStringAppendShort(string, &STR_DECO_UINT16));
            break;
        default:
            return ZYAN_STATUS_INVALID_ARGUMENT;
        }
#endif
        break;
    case ZYDIS_DECORATOR_EH:
#if !defined(ZYDIS_DISABLE_KNC)
        if (context->instruction->avx.has_eviction_hint)
        {
            ZYAN_CHECK(ZydisStringAppendShort(string, &STR_DECO_EH));
        }
#endif
        break;
    default:
        return ZYAN_STATUS_INVALID_ARGUMENT;
    }

    return ZYAN_STATUS_SUCCESS;
}

/* ---------------------------------------------------------------------------------------------- */

/* ============================================================================================== */
