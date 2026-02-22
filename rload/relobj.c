/* Parse RLOAD relocatable object files */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void unknown(int offset, unsigned char value, unsigned expected)
{
    printf("    ? rec[%d] = $%2.2x\n", offset, value);
    if (value != expected)
      printf("*** but normally it's $%2.2x!\n", expected);
}

void expected(int offset, unsigned char value, unsigned expected)
{
    if (value != expected)
        printf("*** rec[%d] = $%2.2x, but expected $%2.2x\n", offset, value, expected);
}

void field(char *dest, char *src, int len)
{
    dest[len] = 0;
    memcpy(dest, src, len);
}

void hexdump(unsigned char *buf, int len)
{
    int i;
    for (i = 0; i != len; ++i)
    {
        printf(" %2.2x", buf[i]);
        if ((i&15) == 15 && i+1 != len)
            printf("\n");
    }
    printf("\n");
}

char *fixup_type_name(unsigned char ty)
{
    if (ty == 0x00) return "word";
    else if (ty == 0x04) return "byte";
    else if (ty == 0x08) return "lbra";
    else return "*** unknown!";
}

char *section_name(unsigned char ty)
{
    if (ty == 0x04) return "psct";
    else if (ty == 0x03) return "dsct";
    else if (ty == 0x02) return "csct";
    else if (ty == 0x01) return "bsct";
    else if (ty == 0x00) return "asct";
    else return "*** unknown!";
}

char *xref_type(unsigned char ty)
{
    if (ty == 0x30) return "any";
    else if (ty == 0x31) return "bsct";
    else if (ty == 0x33) return "dsct";
    else if (ty == 0x34) return "psct";
    else return "*** unknown!";
}

char *xdef_type(unsigned char ty)
{
    if (ty == 0x23) return "dsct";
    else if (ty == 0x20) return "asct";
    else if (ty == 0x21) return "bsct";
    else if (ty == 0x24) return "psct";
    else return "*** unknown!";
}

int first_sym = 0;

void parse_record(int rec_offset, unsigned char *buf, int len)
{
    if (len < 1)
    {
        printf("*** Short record at %d\n", rec_offset);
        return;
    }
    else
    {
        printf("Record type code $%2.2x at offset %d:\n", buf[0], rec_offset);
        switch (buf[0])
        {
            case 0x32: // Start, module name
            {
                char modname[7];
                printf("  Type = Start, module name\n");
                if (len != 10)
                {
                    printf("*** Expected record length to be 10, but it was %d\n", len);
                    return;
                }
                unknown(1, buf[1], 0);
                field(modname, buf+2, 6);
                printf("    Module name '%s'\n", modname);
                unknown(8, buf[8], 0x4f);
                unknown(9, buf[9], 0x42);
                break;
            }
            case 0x33: // Symbol table
            {
                printf("  Type = Symbols\n");
                if (len < 18)
                {
                    printf("*** Expected record length to be at least 18, but it was %d\n", len);
                }
                {
                    int i = 1;
                    if (!first_sym)
                    {
                        first_sym = 1;
                        printf("    ? Header:\n");
                        hexdump(buf+1, 17);

                        unknown(1, buf[1], 0);
                        unknown(2, buf[2], 0);
                        unknown(3, buf[3], 0);
                        unknown(4, buf[4], 0);
                        unknown(5, buf[5], 0);

                        expected(6, buf[6], 1);
                        printf("    Size of section $%2.2x (%s) is %x\n", buf[6], section_name(buf[6]), (buf[7]<<8)+buf[8]);
                        expected(9, buf[9], 2);
                        printf("    Size of section $%2.2x (%s) is %x\n", buf[9], section_name(buf[9]), (buf[10]<<8)+buf[11]);
                        expected(12, buf[12], 3);
                        printf("    Size of section $%2.2x (%s) is %x\n", buf[12], section_name(buf[12]), (buf[13]<<8)+buf[14]);
                        expected(15, buf[15], 4);
                        printf("    Size of section $%2.2x (%s) is %x\n", buf[15], section_name(buf[15]), (buf[16]<<8)+buf[17]);
                        i = 18;
                    }

                    while (i < len)
                    {
                        char name[7];
                        if ((buf[i] & 0xF0) == 0x00 && i+5 <= len)
                        {
                            printf("  ASCT $%2.2x, size = $%4.4x, offset = $%4.4x\n",
                                     buf[i], (buf[i+1]<<8)+buf[i+2], (buf[i+3]<<8) + buf[i+4]);
                            i += 5;
                        }
                        else if ((buf[i] & 0xF0) == 0x30 && i+7 <= len)
                        {
                            field(name, buf + i + 1, 6);
                            printf("  XREF $%2.2x (%s), '%s'\n", buf[i], xref_type(buf[i]), name);
                            i += 7;
                        }
                        else if ((buf[i] & 0xF0) == 0x20 && i+9 <= len)
                        {
                            field(name, buf + i + 1, 6);
                            printf("  XDEF $%2.2x (%s), '%s' offset $%4.4x\n", buf[i], xdef_type(buf[i]), name, (buf[i+7] << 8) + buf[i+8]);
                            i += 9;
                        }
                        else if ((buf[i] & 0xF0) == 0x10 && i+9 <= len)
                        {
                            field(name, buf + i + 1, 6);
                            printf("  COMM $%2.2x (%s), '%s' size = $%4.4x\n",
                                buf[i],
                                section_name(buf[i] & 0x0F),
                                name, (buf[i+7] << 8) + buf[i+8]);
                            i += 9;
                        }
                        else
                        {
                            printf("*** Uknown symbol type %2.2x at offset $%2.2x\n", buf[i], i);
                            break;
                        }
                    }
                    if (i != len)
                    {
                        printf("*** Unknown extra stuff in symbol table at offset $%x\n", i);
                        hexdump(buf+i, len-i);
                    }
                }
                break;
            }
            case 0x34: // Data
            {
                if (len < 6)
                {
                    printf("*** Expected record length to be at least 6, but it was %d\n", len);
                }
                else
                {
                    printf("  Type = Data\n");
                    unknown(1, buf[1], 0);
                    printf("    Section $%2.2x (%s) at offset $%2.2x:\n", buf[2], section_name(buf[2]), (buf[4]<<8) + buf[5]);
                    unknown(3, buf[3], 0);
                    printf("    Contents (len = $%2.2x):\n", len - 6);
                    hexdump(buf+6, len - 6);
                }
                break;
            }
            case 0x35: // Fixup
            {
                int i;
                printf("  Type = Fixups\n");
                hexdump(buf, len);
                for (i = 1; i < len; i += 5)
                {
                    unsigned char fixup_type = buf[i];
                    unsigned short fixup_offset = (buf[i+1]<<8) + buf[i+2];
                    unsigned short symbol = (buf[i+3]<<8) + buf[i+4];
                    printf("    Fixup type=$%2.2x (%s), offset=$%4.4x, symbol=%d\n", fixup_type, fixup_type_name(fixup_type), fixup_offset, symbol);
                }
                if (i != len)
                {
                    printf("*** Fixup payload not a multiple of 5?\n");
                }
                break;
            }
            case 0x36: // End
            {
                printf("  Type = End record\n");
                if (len != 4)
                {
                    printf("Expected record length to be 4 but it was %d\n", len);
                    return;
                }
                else
                {
                    printf("    Start address: Section $%2.2x (%s), Offset $%4.4x\n", buf[1], section_name(buf[1]), (buf[2] << 8) + buf[3]);
                }
                break;
            }
            default:
            {
                printf("*** Record type unknown!\n");
                hexdump(buf, len);
                break;
            }
        }
    }
    printf("\n");
}

void parse_objf(FILE *f)
{
    unsigned char buf[256];
    int foffset = 0;
    int c;
    int len;
    for (;;)
    {
        c = fgetc(f);
        if (c == -1)
        {
            return;
        }
        else if (c == 'D')
        {
            int i;
            int rec_offset = foffset;
            unsigned char cksum = 0;
            /* We found a record */
            ++foffset;
            len = fgetc(f);
            if (len == -1)
            {
                printf("Early EOF right after D at offset %d for record at offset %d\n", foffset, rec_offset);
                return;
            }
            ++foffset;
            /* Load record */
            cksum = len;
            for (i = 0; i != len; ++i)
            {
                c = fgetc(f);
                if (c == -1)
                {
                    printf("Early EOF in middle of payload at offset %d for record at offset %d\n", foffset, rec_offset);
                    return;
                }
                ++foffset;
                cksum += c;
                buf[i] = c;
            }
            if (cksum)
            {
                printf("Bad record checksum ($%2.2x) for record at offset %d\n", cksum, rec_offset);
                return;
            }
            c = fgetc(f);
            if (c == -1)
            {
                printf("Early EOF before 0x0D at offset %d for record at offset %d\n", foffset, rec_offset);
                return;
            }
            else if (c != 13)
            {
                printf("Missing 0x0D at offset %d for record at offset %d\n", foffset, rec_offset);
                return;
            }
            else
            {
                ++foffset;
                /* We have a good record! */
                parse_record(rec_offset, buf, len - 1);
            }
        }
        else if (c == 0)
        {
            /* End of file */
            return;
        }
        else
        {
            printf("Unknown character '%c' $%x at offset %d\n", c, (255 & c), foffset);
            return;
        }
    }
}

int main(int argc, char *argv[])
{
    FILE *objf;
    char *objf_name = 0;
    int x;
    for (x = 1; argv[x]; ++x)
        if (objf_name)
        {
            fprintf(stderr, "Syntax error\n");
            return -1;
        }
        else
        {
            objf_name = argv[x];
        }

    if (!objf_name)
    {
        fprintf(stderr, "Syntax error\n");
        return -1;
    }

    objf = fopen(objf_name, "r");
    if (!objf)
    {
        fprintf(stderr, "Couldn't open %s\n", objf_name);
        return -1;
    }
    parse_objf(objf);
    fclose(objf);
    return 0;
}
