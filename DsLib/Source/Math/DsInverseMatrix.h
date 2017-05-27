#ifndef __DS_INVERSE_MATRIX__
#define __DS_INVERSE_MATRIX__


namespace DsLib
{
	template<typename TYPE>
	bool DsInverseMatrix(const TYPE* src, TYPE* dst, const int n )
	{
		const int max_n=200;
		if( max_n <= n)return false;
	   //int ipv, i, j;
	   TYPE inv_pivot, temp;
	   TYPE big;
	   int pivot_row;// row[max_n];
	   TYPE inv_a[max_n][max_n];
	   TYPE a[max_n][max_n];
   
	   for(int i=0;i<n;i++){
   		  for(int j=0;j<n;j++){
	  		  a[i][j]=src[i*n+j];
   		  }
	   }
   
	   /* ---- 単位行列作成 ---------------------------- */
	   for(int i=0 ; i<n ; i++){
		  for(int j=0 ; j<n ; j++){
			 if(i==j){
				inv_a[i][j]=1.0;
			 }else{
				inv_a[i][j]=0.0;
			 }
		  }
	   }

	   for(int ipv=0 ; ipv < n ; ipv++){
		  /* ---- 最大値探索 ---------------------------- */
		  big=0.0;
		  pivot_row = -1;
		  for(int i=ipv ; i<n ; i++){
			 if(fabs(a[i][ipv]) > big){
				big = fabs(a[i][ipv]);
				pivot_row = i;
			 }
		  }
		  if(big == 0.0){
			  //return(false);
		  }

		  //row[ipv] = pivot_row;

		  /* ---- 行の入れ替え -------------------------- */
		  if( (ipv != pivot_row) && (-1!=pivot_row) ){
			 for(int i=0 ; i<n ; i++){
				temp = a[ipv][i];
				a[ipv][i] = a[pivot_row][i];
				a[pivot_row][i] = temp;          	
				temp = inv_a[ipv][i];
				inv_a[ipv][i] = inv_a[pivot_row][i];
				inv_a[pivot_row][i] = temp;
			 }
		  }

		  /* ---- 対角成分=1(ピボット行の処理) ---------- */
		  inv_pivot = static_cast<TYPE>(1.0)/a[ipv][ipv];
		  for(int j=0 ; j < n ; j++){
			 a[ipv][j] *= inv_pivot;
			 inv_a[ipv][j] *= inv_pivot;          
		  }
      
		  /* ---- ピボット列=0(ピボット行以外の処理) ---- */
		  for(int i=0 ; i<n ; i++){
			 if(i != ipv){
				temp = a[i][ipv];
				for(int j=0 ; j<n ; j++){
				   a[i][j] -= temp*a[ipv][j];
				   inv_a[i][j] -= temp*inv_a[ipv][j];                	
				}
			 }
		  }
	   }
   
		for(int i=0;i<n;i++){
   			for(int j=0;j<n;j++){
	  			dst[i*n+j]=inv_a[i][j];
   			}
		}
		return(true);
	}

	template<const int SIZE, typename TYPE>
	bool DsInverseMatrix(const TYPE src[SIZE][SIZE], TYPE dst[SIZE][SIZE])
	{
		TYPE mat[SIZE][SIZE];
   
		//for(int i=0; SIZE>i ;++i){
   		//	for(int j=0; SIZE>j ;++j){
	  	//		mat[i][j]=src[i][j];
   		//	}
		//}
		memcpy(mat, src, sizeof(TYPE)*SIZE*SIZE);
   
		/* ---- 単位行列作成 ---------------------------- */
		for(int i=0 ; SIZE>i ; ++i){
			for(int j=0 ; SIZE>j ; ++j){
				if(i==j){
					dst[i][j]=1.0;
				}else{
					dst[i][j]=0.0;
				}
			}
		}

		for(int ipv=0 ; SIZE>ipv ; ++ipv){
			
			/* ---- 最大値探索 ---------------------------- */
			TYPE big=0.0;
			int pivot_row=-1;
			for(int i=ipv ; SIZE>i ; ++i){
				if(fabs(mat[i][ipv]) > big){
					big = fabs(mat[i][ipv]);
					pivot_row = i;
				}
			}
			if(big == 0.0){
				//return(false);
			}

			//row[ipv] = pivot_row;

			/* ---- 行の入れ替え -------------------------- */
			if( (ipv != pivot_row) && (-1!=pivot_row) ){
				for(int i=0 ; SIZE>i ; ++i){
					TYPE temp = mat[ipv][i];
					mat[ipv][i] = mat[pivot_row][i];
					mat[pivot_row][i] = temp;          	
					temp = dst[ipv][i];
					dst[ipv][i] = dst[pivot_row][i];
					dst[pivot_row][i] = temp;
				}
			}


			/* ---- 対角成分=1(ピボット行の処理) ---------- */
			if( fabs(mat[ipv][ipv]) >= 0.0000000001)
			{
				const TYPE inv_pivot = 1.0/mat[ipv][ipv];
				for(int j=0 ; j < SIZE ; j++){
					mat[ipv][j] *= inv_pivot;
					dst[ipv][j] *= inv_pivot;          
				}
			}

      
			/* ---- ピボット列=0(ピボット行以外の処理) ---- */
			for(int i=0 ; SIZE>i ; ++i){
				if(i != ipv){
					const TYPE temp = mat[i][ipv];
					for(int j=0 ; SIZE>j ; ++j){
						mat[i][j] -= temp*mat[ipv][j];
						dst[i][j] -= temp*dst[ipv][j];
					}
				}
			}

		}
   
		return(true);
	}


	/*!
	* 4x4行列の行列式の計算
	*  | m[0]  m[1]  m[2]  m[3]  |
	*  | m[4]  m[5]  m[6]  m[7]  |
	*  | m[8]  m[9]  m[10] m[11] |
	*  | m[12] m[13] m[14] m[15] |
	* @param[in] m 元の行列
	* @return 行列式の値
	*/
	template<class TYPE>
	inline TYPE DsCalDetMat16(const TYPE *m)
	{
		return m[0] * m[5] * m[10] * m[15] + m[0] * m[6] * m[11] * m[13] + m[0] * m[7] * m[9] * m[14]
			+ m[1] * m[4] * m[11] * m[14] + m[1] * m[6] * m[8] * m[15] + m[1] * m[7] * m[10] * m[12]
			+ m[2] * m[4] * m[9] * m[15] + m[2] * m[5] * m[11] * m[12] + m[2] * m[7] * m[8] * m[13]
			+ m[3] * m[4] * m[10] * m[13] + m[3] * m[5] * m[8] * m[14] + m[3] * m[6] * m[9] * m[12]
			- m[0] * m[5] * m[11] * m[14] - m[0] * m[6] * m[9] * m[15] - m[0] * m[7] * m[10] * m[13]
			- m[1] * m[4] * m[10] * m[15] - m[1] * m[6] * m[11] * m[12] - m[1] * m[7] * m[8] * m[14]
			- m[2] * m[4] * m[11] * m[13] - m[2] * m[5] * m[8] * m[15] - m[2] * m[7] * m[9] * m[12]
			- m[3] * m[4] * m[9] * m[14] - m[3] * m[5] * m[10] * m[12] - m[3] * m[6] * m[8] * m[13];
	}

	/*!
	* 4x4行列の行列式の計算
	*  | m[0]  m[1]  m[2]  m[3]  |
	*  | m[4]  m[5]  m[6]  m[7]  |
	*  | m[8]  m[9]  m[10] m[11] |
	*  | m[12] m[13] m[14] m[15] |
	* @param[in] m 元の行列
	* @param[out] invm 逆行列
	* @return 逆行列の存在
	*/
	template<typename TYPE>
	inline bool DsInverseMatrixDirect16(const TYPE *m, TYPE *invm)
	{
		const TYPE RX_FEQ_EPS = static_cast<TYPE>(0.00000001);

		TYPE det = DsCalDetMat16(m);
		if (static_cast<TYPE>(fabs(det)) < RX_FEQ_EPS)
		{
			return false;
		}
		else
		{
			TYPE inv_det = static_cast<TYPE>(1.0 / det);

			invm[0] = inv_det*(m[5] * m[10] * m[15] + m[6] * m[11] * m[13] + m[7] * m[9] * m[14] - m[5] * m[11] * m[14] - m[6] * m[9] * m[15] - m[7] * m[10] * m[13]);
			invm[1] = inv_det*(m[1] * m[11] * m[14] + m[2] * m[9] * m[15] + m[3] * m[10] * m[13] - m[1] * m[10] * m[15] - m[2] * m[11] * m[13] - m[3] * m[9] * m[14]);
			invm[2] = inv_det*(m[1] * m[6] * m[15] + m[2] * m[7] * m[13] + m[3] * m[5] * m[14] - m[1] * m[7] * m[14] - m[2] * m[5] * m[15] - m[3] * m[6] * m[13]);
			invm[3] = inv_det*(m[1] * m[7] * m[10] + m[2] * m[5] * m[11] + m[3] * m[6] * m[9] - m[1] * m[6] * m[11] - m[2] * m[7] * m[9] - m[3] * m[5] * m[10]);

			invm[4] = inv_det*(m[4] * m[11] * m[14] + m[6] * m[8] * m[15] + m[7] * m[10] * m[12] - m[4] * m[10] * m[15] - m[6] * m[11] * m[12] - m[7] * m[8] * m[14]);
			invm[5] = inv_det*(m[0] * m[10] * m[15] + m[2] * m[11] * m[12] + m[3] * m[8] * m[14] - m[0] * m[11] * m[14] - m[2] * m[8] * m[15] - m[3] * m[10] * m[12]);
			invm[6] = inv_det*(m[0] * m[7] * m[14] + m[2] * m[4] * m[15] + m[3] * m[6] * m[12] - m[0] * m[6] * m[15] - m[2] * m[7] * m[12] - m[3] * m[4] * m[14]);
			invm[7] = inv_det*(m[0] * m[6] * m[11] + m[2] * m[7] * m[8] + m[3] * m[4] * m[10] - m[0] * m[7] * m[10] - m[2] * m[4] * m[11] - m[3] * m[6] * m[8]);

			invm[8] = inv_det*(m[4] * m[9] * m[15] + m[5] * m[11] * m[12] + m[7] * m[8] * m[13] - m[4] * m[11] * m[13] - m[5] * m[8] * m[15] - m[7] * m[9] * m[12]);
			invm[9] = inv_det*(m[0] * m[11] * m[13] + m[1] * m[8] * m[15] + m[3] * m[9] * m[12] - m[0] * m[9] * m[15] - m[1] * m[11] * m[12] - m[3] * m[8] * m[13]);
			invm[10] = inv_det*(m[0] * m[5] * m[15] + m[1] * m[7] * m[12] + m[3] * m[4] * m[13] - m[0] * m[7] * m[13] - m[1] * m[4] * m[15] - m[3] * m[5] * m[12]);
			invm[11] = inv_det*(m[0] * m[7] * m[9] + m[1] * m[4] * m[11] + m[3] * m[5] * m[8] - m[0] * m[5] * m[11] - m[1] * m[7] * m[8] - m[3] * m[4] * m[9]);

			invm[12] = inv_det*(m[4] * m[10] * m[13] + m[5] * m[8] * m[14] + m[6] * m[9] * m[12] - m[4] * m[9] * m[14] - m[5] * m[10] * m[12] - m[6] * m[8] * m[13]);
			invm[13] = inv_det*(m[0] * m[9] * m[14] + m[1] * m[10] * m[12] + m[2] * m[8] * m[13] - m[0] * m[10] * m[13] - m[1] * m[8] * m[14] - m[2] * m[9] * m[12]);
			invm[14] = inv_det*(m[0] * m[6] * m[13] + m[1] * m[4] * m[14] + m[2] * m[5] * m[12] - m[0] * m[5] * m[14] - m[1] * m[6] * m[12] - m[2] * m[4] * m[13]);
			invm[15] = inv_det*(m[0] * m[5] * m[10] + m[1] * m[6] * m[8] + m[2] * m[4] * m[9] - m[0] * m[6] * m[9] - m[1] * m[4] * m[10] - m[2] * m[5] * m[8]);

			return true;
		}
	}



}
#endif