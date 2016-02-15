#include <stdio.h>
#include <Windows.h>

FILE *Encrypt(char *input, char *output, char *array, char *key)
{
    FILE *result;
    FILE *result2;
    signed int v5 = 0;
    int v7;
    fopen_s(&result, input, "rb");
    fopen_s(&result2, (const char *)output, "w");
    fprintf(result2, "//const\nunsigned char %s[] = {", array);
    srand(atoi(key));

    while (1) {
        v7 = getc(result);

        if (v7 == -1)
            break;

        if (v5)
            fprintf(result2, ", ");

        if (!(v5 % 16))
            fprintf(result2, "\n\t");

        v7 = v7 ^ atoi(key);
        srand(atoi(key));
        fprintf(result2, "0x%.2X", (unsigned __int8)(BYTE)v7);
        ++v5;
    }

    fprintf(result2, "\n};\n");
    fclose(result);
    fclose(result2);
    return result2;
}
//////////////////////////////////////////////////////////////////////////
int main(int argc, char** argv)
{
    //system("cls");
    printf("[+] EncryptFile V2.0.0 by Eric21 \n", argv[0]);
    printf("[+] www.eric21.com \n", argv[0]);

    if (argc < 2) {
        printf("[Usage]:\n");
        printf("%s [input_file] [output_file] [array_name] [key]\n\n", argv[0]);
        printf("[Example]:\n");
        printf("%s test.exe test.h 0x12\n", argv[0]);
        printf("%s test.exe test.h test 0x12\n", argv[0]);
        exit(-1);
    } else {
        //argv[3] = (int)"filedata";
        printf("[*] [input_file]:  %s\n", argv[1]);
        printf("[*] [output_file]: %s\n", argv[2]);
        printf("[*] [array_name]:  %s\n", argv[3]);
        printf("[*] [key]:  %s\n", argv[4]);
        printf("\n");

        if (Encrypt(argv[1], argv[2], argv[3], argv[4]))
            printf("[+] EncryptFile(); Successfully! ^_^\n");
        else
            printf("[-] EncryptFile(); Failed.\n");
    }

    return 0;
}
