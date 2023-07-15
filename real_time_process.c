#include "motoPlus.h"

void mpTask1();
void mpTask2();

// GLOBAL DATA DEFINITIONS
int nTaskID1;
int nTaskID2;

static int handOn(void);
static int handOff(void);

void mpUsrRoot(int arg1, int arg2, int arg3, int arg4, int arg5,int arg6, int arg7, int arg8, int arg9, int arg10){
    //TODO: Add additional intialization routines
    //Creates and starts a new task in a seperate thread of execution.

// All arguments will be passed to the new task if the function
//prototype will accept them
nTaskID1 = mpCreateTask(MP_PRI_TIME_NORMAL, MP_STACK_SIZE, (FUNCPTR)mpTask1, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10);
nTaskID2 = mpCreateTask(MP_PRI_TIME_NORMAL, MP_STACK_SIZE, (FUNCPTR)mpTask2, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10);
//Ends the initialization task.
    mpExitUsrRoot;
}

void mpTask1(void){
    int rc;
    int i;
    int id;
    int timeout, delay;
    int grpNo;
    MP_SPEED spd;
    MP_TARGET target;
    MP_USR_VAR_INFO varInfo;

    // timeout = 30[sec]
    timeout = 30000 / mpGetRtc();
    delay = 1000 / mpGetRtc();

    //TODO: Add the code for this task
    FOREVER{
        memset(&varInfo,0,sizeof(varInfo));
        varInfo.var_type = MP_VAR_B;
        varInfo.var_no = 0;
        // waiting for start event.
        printf("\n");
        printf("... waiting for start event.\n");
        while (varInfo.val.b == 0){
            mpTaskDelay(delay);
            if ((rc = mpGetUserVars(&varInfo)) < 0)
                break;
        }
        if (rc < 0){
            printf("%d = mpGetUserVars(MP_VAR_B)\n", rc);
            continue;
        }
        
        varInfo.val.b = 0; // clear event
        mpPutUserVars(&varInfo);

        // get group No.
        if((grpNo = mpCtrlGrpld2GrpNo(MP_R1_GID))<0){
            printf("%d = mpCtrlGrpld2GrpNo(MP_R1_GID)\n", grpNo);
            continue;
        }
        
        // initialize motion control.
        mpMotStop(0);
        mpMotTargetClear(0x0f,0);

        // send targets.
        if ((rc = mpMotSetCoord(grpNo, MP_BASE_TYPE, 0)) <0){
            printf("%d = mpMotSetCoord()\n",rc);
            continue;
        }
        
        memset(&spd,0,sizeof(spd));
        spd.v = 500; // 50.0[mm/sec]
        if ((rc = mpMotSetSpeed(grpNo, &spd)) < 0){
            printf("%d = mpMotSetSpeed()\n", rc);
            continue;
        }
        for (i = 1; i < 6; i++){
            printf("send No.%d target.\n", i);
            memset(&target, 0, sizeof(target));
            target.id = i;
            target.intp = MP_MOVL_TYPE;
            varInfo.var_type = MP_VAR_P;
            varInfo.var_no = i;
            if ((rc = mpGetUserVars(&varInfo)) < 0){
                printf("%d = mpGetUserVars(MP_VAR_P)\n", rc);
                break;
            }
            else{
                memcpy(target.dst.joint, varInfo.val.p.data, sizeof(target.dst.joint));
            }
            
            if ((rc = mpMotTargetSend((1 << grpNo), &target, timeout)) < 0)
            {
                printf("%d = mpMotTargetSend()\n",rc); 
                break;
            }
        }
            
        if (rc < 0){
            printf("sending target failed.\n");
            continue;
        }
        
        handOff(); // open the hand.
        printf("\n");
        printf("count down ...\n");
        for (i = 5;i> 0;i--){
            printf("%d\n", i);
            mpTaskDelay(delay);
        }

        // motion start!
        printf("motion start!\n");
        if ((rc = mpMotStart(0)) < 0){
            printf("%d = mpMotStart()\n", rc);
            continue;
        }
        
        // ... waiting for target id = 1.
        id = 1;
        printf("... waiting for target id = %d.\n", id);
        if((rc = mpMotTargetReceive(grpNo,id, NULL, timeout, 0)) < 0){
            printf("%d =mpMotTargetReceive(n",rc);
            continue;
        }
        printf("arriving to target id = %d.\n", id);
        printf("close the hand!\n");
        handOn();// close the hand.

        // ... waiting for target id = 4
        id = 4;
        printf("... waiting for target id = %d.\n", id);
        if((rc = mpMotTargetReceive(grpNo, id, NULL, timeout, 0)) < 0){
            printf("%d =mpMotTargetReceive()\n", rc);
            continue;
        } 
        printf("arriving to target id = %d.\n", id);
        printf("open the hand!\n");
        handOff();// open the hand.

        // ... waiting for target id = 5
        id = 5;
        printf("... waiting for target id = %d.\n", id);
        if((rc = mpMotTargetReceive(grpNo, id, NULL, timeout, 0)) < 0){
            printf("%d = mpMotTargetReceive()\n",rc);
            continue;
        }
        printf("arriving to target id = %d.\n", id);
        
        printf("\n");
        printf("return to home position count down ...\n");
        for (i = 5;i> 0;i--){
            printf("%d\n",i);
            mpTaskDelay(delay);
        }
        
        // send home position.
        if ((rc = mpMotSetCoord(grpNo, MP_ANGLE_TYPE, 0))<0){
            printf("%d = mpMotSetCoord()\n",rc);
            continue;
        }
        
        memset(&spd,0, sizeof(spd));
        spd.vj = 1000; // 10.00[%]
        if ((rc = mpMotSetSpeed(grpNo, &spd)) < 0){
            printf("%d = mpMotSetSpeed()\n", rc);
            continue;
        }
        
        printf("send home position.\n");
        memset(&target,0, sizeof(target));
        target.id = id = 0;
        target.intp = MP_MOVJ_TYPE;
        varInfo.var_type = MP_VAR_P;
        varInfo.var_no = 0;
        if ((rc = mpGetUserVars(&varInfo)) < 0){
            printf("%d = mpGetUserVars(MP_VAR_P)\n",rc);
            continue;
        }
        else{
            memcpy(target.dst.joint, varInfo.val.p.data, sizeof(target.dst.joint));
        }
        
        if ((rc = mpMotTargetSend((1 << grpNo), &target, timeout)) < 0){
            printf("%d = mpMotTargetSend()\n", rc);
            break;
        }
        
        if ((rc = mpMotTargetReceive(grpNo, id, NULL, timeout, 0)) < 0){
            printf("%d = mpMotTargetReceive()n", rc);
            continue;
        }
        printf("complete!\n");
        mpTaskDelay(delay);
    }
}

static int handOn(void){
    MP_IO_DATA req;
    req.ulAddr = 10010;
    req.ulValue = 1;
    return (mpWritelO(&req, 1));
}


static int handOff(void){
    MP_IO_DATA req;
    req.ulAddr =10010;
    req.ulValue = 0;
    return (mpWritelO(&req, 1));
}

void mpTask2(int arg1, int arg2){
    //TODO: Add the code for this task
}

