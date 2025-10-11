#ifndef FOLLOW_H
#define FOLLOW_H

enum {
    PEAK = 0,
    RMS
};

enum {
    PERPOINT = 0,
    TWOPOINT
};

typedef struct _FollowParam {
    int SampleRate;
    int channel;
    int attackTime;
    int releaseTime;
    int rmsTime;
    short algorithm;
    short mode;
} FollowParam;

typedef struct _FollowProcess {
    void(*Process)(void *workBuf, short in, int per_channel_npoint);
} FollowProcess;

typedef struct _FollowRunProcess {
    int(*Run)(void *workBuf, short *in, int per_channel_npoint);
} FollowRunProcess;

typedef struct _Follow {
    FollowProcess process;
    FollowRunProcess run;
    int attFactor;
    int relFactor;
    int channel;
    int SampleRate;
    int rmsLen;
    short algorithm;
    short mode;

    long long rmsSum[2];
    int rmsIdx[2];
    int follow[2];
    short db[2];
    short *rmsbuf[2];
    short mempool[0];
} Follow;

int getFollowBuf(FollowParam *param);
short *getFollowDB(void *workbuf);
int FollowInit(void *workbuf, FollowParam *param);
int FollowRun(void *workbuf, short *in, int per_channel_npoint);
#endif // !FOLLOW_H

