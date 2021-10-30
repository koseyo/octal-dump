// ヘッダーファイル読み込み
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define OCTAL 0
#define HEX 1
#define MAX_CHARACTER 10000 // 文字を入れるための配列の添字

// プロトタイプ宣言
int fSize(char *fname); // ファイルポインタを引数にしてファイルサイズを返す関数
void od(char *fname, int file_size, int flag); // ダンプ処理を行うための関数

// メイン関数
int main(int argc, char *argv[]) {
    char *fname;
    int flag = OCTAL; // 処理切り替えのための変数を用意

    if(argc < 2 || 3 < argc) { // 引数が多すぎたらストップ
        printf("引数の数が多すぎます\n");
        return 1;
    }
    
    for(int i = 1; i < argc; i++) { //　コマンドラインの数だけループする

        if(strncmp(argv[i], "-x", 2) == 0) { // コマンドライン引数に-xがあればヘックスダンプを行う
            fname = argv[i + 1]; // コマンドラインに入力されたファイル名をポインタへ格納する
            if(fname == NULL) return 1;
            flag = HEX; // 一部処理を16進数で行うための変数を用意
        } else { // オクタルダンプを行う場合
            fname = argv[1];
        }

        if(fname == NULL) return 1;
        int file_size = fSize(fname); // ファイルサイズを関数で取得し、変数へ格納する
        od(fname, file_size, flag); // ダンプ関数を呼び出す
        break; // HEXでの処理の場合、何故かsegmentation fault 11 が発生。原因が判明しないため、それを避けるためやむなく記述
    }

    return 0;
}

// ファイルサイズを返す関数
int fSize(char *fname) {
    FILE *fp;
    fp = fopen(fname, "rb"); // ファイルオープン
    int count = 0; // ファイルサイズをカウントするための変数

    while(1) {
        if(fgetc(fp) == EOF) { // 読み込みが終わればループ終了
            break;
        }
    count++; // 1ループごとに1バイト増え続ける
    }

    fclose(fp); // ファイルクローズ
    return count; // ファイルサイズを返り値として送る
}

// ダンプ処理をするための関数
void od(char *fname, int file_size, int flag) {
    char buff[MAX_CHARACTER]; // 文字を入れるための配列(添字は10000)
    FILE *fp; // ファイルのポインタ
    fp = fopen(fname, "rb"); // バイナリファイルで読み込む
    int offset = 0; // 左端の列の数字を表示するための変数

    for (int i = 0; i < file_size; i++) {
        if (i % 16 == 0) { // 16の倍数毎に
            printf("%07d    ", offset); // 左端の列にオフセットを表示
            offset += 10; // 10の倍数で増えていく
        }

        if (i % 16 != 0 && i % 8 == 0) {
            printf("  "); // 8バイトずつ真ん中に空白を入れていく
        }

        fread(&buff[i], 1, 1, fp); // 1バイトずつファイル読み込み

        if (buff[i] <= 0x1f || buff[i] == 0x7f) { // 特殊文字の場合
            printf((flag == OCTAL) ? "... " : ".. "); // "."を表示
        } else {
            printf((flag == OCTAL) ? "%03o " : "%02x ", buff[i]); // 8進数or16進数で表示
        }
 
        if (i == file_size - 1 && i % 16 != 15) { // 最終列の処理
            for (i++; i < ((file_size + 16) & 0xfff0); i++) { // 16バイトアライメントすることでループを終了させる
                if (i % 16 != 0 && i % 8 == 0) {
                    printf("  "); // 8バイトずつ真ん中に空白を入れていく
                }
                printf((flag == OCTAL) ? "... " : ".. "); // ファイルが終わったら ... で埋めていく
            }
            i--;
        }
 
        if (i % 16 == 15) { // 16行ダンプ表示したら
            printf("  "); // 空白を入れる

            for (int j = i - 15; j <= i; j++) { // iと比較して0~15回ループさせる
                if (j >= file_size) { // ファイルサイズを超えたら
                    break; // 処理終了
                }

                if (buff[j] <= 0x1f || buff[j] == 0x7f) { // 特殊文字の場合
                    printf("."); // "."を表示
                } else {
                    printf("%c", buff[j]); // 実際の文字を表示
                }
            }

            printf("\n");
        }
    }
    
    printf("\n");
    fclose(fp);
}