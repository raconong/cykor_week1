#include <stdio.h>
#define STACK_SIZE 50 // 최대 스택 크기

int     call_stack[STACK_SIZE];         // Call Stack을 저장하는 배열
char    stack_info[STACK_SIZE][20];     // Call Stack 요소에 대한 설명을 저장하는 문자열 배열

int SP = -1; 
int FP = -1;

// 스택 조작 함수
void push_stack(int value, const char* info)
{
    call_stack[++SP] = value;
    sprintf(stack_info[SP], "%s", info);
}

void pop_stack(int count)
{
    SP -= count;
}

void restore_fp()
{
    SP = FP;
    FP = call_stack[SP];
    SP--;
}

void print_stack()
{
    if (SP == -1)
    {
        printf("Stack is empty.\n");
        return;
    }

    printf("====== Current Call Stack ======\n");
    for (int i = SP; i >= 0; i--)
    {
        if (call_stack[i] != -1)
            printf("%d : %s = %d", i ,stack_info[i], call_stack[i]);
        else
            printf("%d : %s", i, stack_info[i]);

        if (i == SP)
            printf("    <=== [esp]\n");
        else if (i == FP)
            printf("    <=== [ebp]\n");
        else
            printf("\n");
    }
    printf("================================\n\n");
}

// 공통 프레임 생성 함수
void create_frame(int argc, int arg[], const char* argnames[],
                  const char* sfp,
                  int localc, int local[], const char* localnames[])
{
    for (int i = argc - 1; i >= 0; i--)
        push_stack(arg[i], argnames[i]);
    push_stack(-1, "Return Address");
    push_stack(FP, sfp);
    FP = SP;
    for (int i = 0; i < localc; i++)
        push_stack(local[i], localnames[i]);
}

// 공통 프레임 제거 함수
void remove_frame(int localc, int argc)
{
    pop_stack(localc);      // 지역변수 제거
    restore_fp();           // FP 복원
    pop_stack(argc + 1);    // 매개변수 + Return Address 제거
}

// 함수 선언
void func1(int arg1, int arg2, int arg3);
void func2(int arg1, int arg2);
void func3(int arg1);

// func1 정의

void func1(int arg1, int arg2, int arg3)
{
    int var_1 = 100;

    int argv[] = {arg1, arg2, arg3};
    const char* argnames[] = {"arg1", "arg2", "arg3"};
    int localv[] = {var_1};
    const char* localnames[] = {"var_1"};

    // func1의 스택 프레임 형성 (함수 프롤로그 + push)
    create_frame(3, argv, argnames, "func1 SFP", 1, localv, localnames); //func1 : 매개변수 3개, 지역변수 1개개
    print_stack();
    func2(11, 13);
    // func2의 스택 프레임 제거 (함수 에필로그 + pop)
    remove_frame(1, 2); // 지역변수 1개, 매개변수 2개 
    print_stack();
}

// func2 정의
void func2(int arg1, int arg2)
{   
    int var_2 = 200;

    int argv[] = {arg1, arg2};
    const char* argnames[] = {"arg1", "arg2"};
    int localv[] = {var_2};
    const char* localnames[] = {"var_2"};

    create_frame(2, argv, argnames, "func2 SFP", 1, localv, localnames); //func2 생성 : 매개변수 2개, 지역변수 1개 
    print_stack();

    func3(77);

    remove_frame(2, 1); //func3제거 : 지역 2개, 매개변수 1개 
    print_stack();
}

// func3 정의
void func3(int arg1)
{

    int var_3 = 300;
    int var_4 = 400;


    int argv[] = {arg1};
    const char* argnames[] = {"arg1"};
    int localv[] = {var_3, var_4};
    const char* localnames[] = {"var_3", "var_4"};

    create_frame(1, argv, argnames, "func3 SFP", 2, localv, localnames); //매개변수 1개, 지역변수 2개 
    print_stack();
}

// main 함수
int main()
{
    func1(1, 2, 3);

    // func1 프레임 제거
    remove_frame(1, 3);  // 매개변수 3개, 지역변수 1개
    print_stack();
    return 0;
}
