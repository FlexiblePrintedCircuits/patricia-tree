#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHAR_SIZE 26

struct Patricia
{
    int isLeaf;
    struct Patricia *children[CHAR_SIZE];
    char *str;
};

struct Patricia *getNewPatriciaNode()
{
    struct Patricia *node = (struct Patricia *)malloc(sizeof(struct Patricia));
    node->isLeaf = 0;
    node->str = "";

    for (int i = 0; i < CHAR_SIZE; i++)
    {
        node->children[i] = NULL;
    }

    return node;
}

// 追加。先頭文字が一致するか調べ、一致したら分割可能と判断する。
int can_divide(char *str1, char *str2) {
    return (str1[0] == str2[0]) ? 1 : 0;
}

// 追加。ノードを分割する際、どの位置で分割すれば良いのか返す。
// 例えばtryとtriedなら、(1から数えて)3文字目で分割できるので、(0から数えて)2を返す。
int position_to_split(char *str1, char *str2)
{
    for (int i = 0; i < strlen(str1); i++) {
        if (str1[i] == str2[i])
        {
            continue;
        }
        else
        {
            return i;
        }
    }
    return 0;
}

struct Patricia *divide_node(struct Patricia *root, char *str)
{
    // もしもposition_to_splitがNULLなら、root->strの末端以降の文字列を新ノードとして追加する。rootのisLeafが1なら0にする。
    // 例えばcarのノードにcardを追加したいなら、新しくdのノードを追加する。isLeafを1にする。
    if (!position_to_split(root->str, str))
    {
        int len = strlen(root->str);
        int split_len = strlen(str) - len;
        char *substring = (char*)malloc(sizeof(char)*(split_len + 1));
        strncpy(substring, str + len, split_len);
        substring[split_len] = '\0';

        root->children[substring[0] - 'a'] = getNewPatriciaNode();
        root->children[substring[0] - 'a']->str = substring;
        root->children[substring[0] - 'a']->isLeaf = 1;

        if (root->isLeaf)
        {
            root->isLeaf = 0;
        }

        printf("Add \"%s\" node.\n", root->children[substring[0] - 'a']->str);

        return root;
    }
    else
    {
        // NULLでなければ、position_to_splitの返ってきた値までの所でroot->strをスライスし、root->strに再代入する。もしisLeafが1なら0にする。
        // rootの子ノードに、先ほどスライスして捨てられた部分の文字列の新ノードを追加する。例えばtryかtrで分割されたなら、yのノードを追加する。isLeafを1にする。
        // rootの子ノードに、strのスライスされた文字列以降の文字列の新ノードを追加する。例えばtriedを追加したくて、trで分割されたならiedのノードを追加する。isLeafを1にする。

        // 分割すべき場所を取得する
        int position = position_to_split(root->str, str);

        // 分割すべき場所から、何文字目までをスライスするのかを計算する
        int first_childnode_split_position = strlen(root->str) - position;
        int second_childnode_split_position = strlen(str) - position;

        // 再代入したり追加する、ノードの文字列用の変数
        char *new_root_str = (char *)malloc(sizeof(char) * (position + 1));
        char *first_childnode_str = (char *)malloc(sizeof(char) * (first_childnode_split_position + 1));
        char *second_childnode_str = (char *)malloc(sizeof(char) * (second_childnode_split_position + 1));

        // 再代入したり追加する、ノードの文字列のスライス
        strncpy(new_root_str, root->str, position);
        strncpy(first_childnode_str, root->str + position, first_childnode_split_position);
        strncpy(second_childnode_str, str + position, second_childnode_split_position);

        // 終端文字をつける
        new_root_str[position] = '\0';
        first_childnode_str[first_childnode_split_position] = '\0';
        second_childnode_str[second_childnode_split_position] = '\0';

        // ルートノードの更新
        printf("Update \"%s\" ", root->str);
        root->str = new_root_str;
        if (root->isLeaf)
        {
            root->isLeaf = 0;
        }
        printf("to \"%s\". \n", root->str);

        // ノードの分割
        root->children[first_childnode_str[0] - 'a'] = getNewPatriciaNode();
        root->children[first_childnode_str[0] - 'a']->str = first_childnode_str;
        root->children[first_childnode_str[0] - 'a']->isLeaf = 1;

        root->children[second_childnode_str[0] - 'a'] = getNewPatriciaNode();
        root->children[second_childnode_str[0] - 'a']->str = second_childnode_str;
        root->children[second_childnode_str[0] - 'a']->isLeaf = 1;

        printf("Add \"%s\" node.\n", root->children[first_childnode_str[0] - 'a']->str);
        printf("Add \"%s\" node.\n", root->children[second_childnode_str[0] - 'a']->str);

        return root;
    }
}

// 変更した。
void insert(struct Patricia *head, char *str)
{
    printf("***** Start process of inserting \"%s\" node *****\n", str);

    struct Patricia *curr = head;

    for (int i = 0; i < CHAR_SIZE; i++) // 全ての子ノードを調べる
    {
        if (curr->children[i]) // もしもNULLではない子ノードがあったら
        {
            if (can_divide(curr->children[i]->str, str)) // もしも子ノードの文字列と、新しく追加したい文字列が分割できるのであれば
            {
                curr->children[i] = divide_node(curr->children[i], str);
                printf("***** End process of inserting \"%s\" node *****\n\n", str);
                return;
            }
        }
    }

    // ↓もし部分一致する子ノードが１つも無かったら、str全文が入るノードを新しく用意する。
    curr->children[str[0] - 'a'] = getNewPatriciaNode();
    curr->children[str[0] - 'a']->str = str;
    curr->children[str[0] - 'a']->isLeaf = 1;
    printf("Add \"%s\" node.\n", curr->children[str[0] - 'a']->str);

    printf("***** End process of inserting \"%s\" node *****\n\n", str);
}

int search(struct Patricia *root, char *str)
{
    // strに、今いるノードの文字列(node_strとする)は存在するか確かめる。①
    // もし存在して、かつisLeafが0なら次のノードにいき、①をする
    // もし存在して、かつisLeafが1なら引数として1を返す。
    // もし存在しないなら、引数として0を返す。

    struct Patricia *curr = root;
    curr = curr->children[str[0] - 'a'];
    while (1)
    {
        if (strstr(str, curr->str)) // ノードの文字列は、検索対象の文字列の部分文字列？
        {
            if (curr->isLeaf)   // 葉か？
            {
                return 1;   // 葉まで到達したのであれば検索対象の文字列は存在している
            }
            else if ((curr->isLeaf) == 0)   // 葉ではないか？
            {
                str = str + strlen(curr->str) + 1;  // 次の検索対象の文字を探す。cardならd
                if (curr->children[str[0] - 'a'])   // NULLではないか？
                {
                    curr = curr->children[str[0] - 'a'];    // 次の検索対象の文字を代入
                    continue;   // 次のループへ
                }
                else
                    return 0;    // NULLだったら検索対象の文字列は存在しない
            }
        }
        else
            return 0;    // ノードの文字列が検索対象の文字列の部分文字列でなければ、検索対象の文字列は存在しない
    }

    return 0;
}

int hasChildren(struct Patricia *curr)
{
    for (int i = 0; i < CHAR_SIZE; i++)
    {
        if (curr->children[i])
        {
            return 1;
        }
    }

    return 0;
}

int deletion(struct Patricia **curr, char *str)
{
    if (*curr == NULL)
    {
        return 0;
    }

    if (*str)
    {
        if (*curr != NULL && (*curr)->children[*str - 'a'] != NULL &&
            deletion(&((*curr)->children[*str - 'a']), str + 1) &&
            (*curr)->isLeaf == 0)
        {
            if (!hasChildren(*curr))
            {
                free(*curr);
                (*curr) = NULL;
                return 1;
            }
            else
            {
                return 0;
            }
        }
    }

    if (*str == '\0' && (*curr)->isLeaf)
    {
        if (!hasChildren(*curr))
        {
            free(*curr);
            (*curr) = NULL;
            return 1;
        }

        else
        {
            (*curr)->isLeaf = 0;
            return 0;
        }
    }

    return 0;
}

int main()
{
    struct Patricia *head = getNewPatriciaNode();
    insert(head, "car");
    insert(head, "card");
    insert(head, "try");
    insert(head, "tried");
    insert(head, "cat");

    if (search(head, "card"))
        printf("card is exist\n");
    else
        printf("card is not exist\n");

    if (search(head, "carp"))
        printf("carp is exist\n");
    else
        printf("carp is not exist\n");

    if (search(head, "trie"))
        printf("trie is exist\n");
    else
        printf("trie is not exist\n");

    return 0;

    /*
    insert(head, "hello");
    printf("%d ", search(head, "hello")); // print 1

    insert(head, "helloworld");
    printf("%d ", search(head, "helloworld")); // print 1

    printf("%d ", search(head, "helll")); // print 0 (Not present)

    insert(head, "hell");
    printf("%d ", search(head, "hell")); // print 1

    insert(head, "h");
    printf("%d \n", search(head, "h")); // print 1 + newline

    deletion(&head, "hello");
    printf("%d ", search(head, "hello"));      // print 0 (hello deleted)
    printf("%d ", search(head, "helloworld")); // print 1
    printf("%d \n", search(head, "hell"));     // print 1 + newline

    deletion(&head, "h");
    printf("%d ", search(head, "h"));           // print 0 (h deleted)
    printf("%d ", search(head, "hell"));        // print 1
    printf("%d\n", search(head, "helloworld")); // print 1 + newline

    deletion(&head, "helloworld");
    printf("%d ", search(head, "helloworld")); // print 0
    printf("%d ", search(head, "hell"));       // print 1

    deletion(&head, "hell");
    printf("%d\n", search(head, "hell")); // print 0 + newline

    if (head == NULL)
    {
        printf("Trie empty!!\n"); // Trie is empty now
    }

    printf("%d ", search(head, "hell")); // print 0
    */
}
