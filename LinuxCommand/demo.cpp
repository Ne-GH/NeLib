#include <iostream>
#include <string>


#include "myls/myls.h"
#include "myls/myls_cpp.h"

#include "mypwd/mypwd.h"
#include "mypwd/mypwd_cpp.h"
#include "myyes/myyes.h"
/*********************************************************************
 * myls
*********************************************************************/
void myls(){
    Getname();
}

/*********************************************************************
 * myyes
*********************************************************************/
void myyes(int argc,char **argv){
    GetStr(argc, argv);
Go:
    write(1,buf,BUFFERSIZE);
    goto Go;

}


/*********************************************************************
 * mypwd
*********************************************************************/
void mypwd(){
    GetPwd();
}


int main(int args,char *argc[]){

    std::cout << MyLsCpp17() << std::endl;
    std::cout << MyPwdCpp17() << std::endl;
    // myls();

	return 0;
}
