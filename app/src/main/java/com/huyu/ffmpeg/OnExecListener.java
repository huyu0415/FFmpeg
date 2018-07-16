package com.huyu.ffmpeg;

interface OnExecListener {

	void onExecutedProgress(float result);

	void onExecutedError(int result);

	void onExecutedSuccess();

}
