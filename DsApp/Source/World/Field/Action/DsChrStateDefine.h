#pragma once


namespace DsApp
{
	namespace DsChrStateDefine
	{
		const int CHR_STATE_GRAPH_ID = -1;//パラメータからは指定されない
		const int PLAYER_STATE_GRAPH_ID = 0;
		const int ENEMY1_STATE_GRAPH_ID = 1;

		//staticな処理の初期化。２回目以降叩いても何もしない
		void InitializeState();


	}
}