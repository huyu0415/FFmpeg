//
// Created by huyu on 2018/5/7.
//

#ifndef CODECTRL_LIVEPLAYER_H
#define CODECTRL_LIVEPLAYER_H

int cameraLivePlayerInit(int width, int height, const char *urlpath);

int cameraLivePlayerStart(signed char *yuv);

int cameraLivePlayerStop();

int cameraLivePlayerClose();

#endif //CODECTRL_LIVEPLAYER_H
