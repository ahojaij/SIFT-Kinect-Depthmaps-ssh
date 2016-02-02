#include "InteractionMatrix.h"

double** getL_Z(USHORT * depth2, double* L_x, double* L_y, double* L_P_Z, double dZ_dx, double dZ_dy, double** L_Z)
{
	for(int i=0; i<480; ++i)
	{
		for(int j=0; j<640; ++j)
		{
			L_x[0] = -1/(double)depth2[i*640+j];
			L_x[1] = 0;
			L_x[2] = j/(double)depth2[i*640+j];
			L_x[3] = j*i;
			L_x[4] = -(1+j*j);
			L_x[5] = i;

			L_y[0] = 0;
			L_y[1] = -1/(double)depth2[i*640+j];
			L_y[2] = i/(double)depth2[i*640+j];
			L_y[3] = -(1+i*i);
			L_y[4] = -j*i;
			L_y[5] = -j;

			L_P_Z[0] = 0;
			L_P_Z[1] = 0;
			L_P_Z[2] = -1;
			L_P_Z[3] = -i*(double)depth2[i*640+j];
			L_P_Z[4] = i*(double)depth2[i*640+j];
			L_P_Z[5] = 0;

			if (j<639)
				dZ_dx = (double)depth2[i*640+j+1]-(double)depth2[i*640+j];
			if (i<479)
				dZ_dy = (double)depth2[(i+1)*640+j]-(double)depth2[i*640+j];
			double a;
			for (int k=0; k<6; ++k)
			{
				L_Z[i*640+j][k] = L_P_Z[k] - dZ_dx * L_x[k] - dZ_dy * L_y[k];
				a = L_Z[i*640+j][k];
			}
		}
	}
	return L_Z;
}


double** getL_Z_trans(double** L_Z_trans, double** L_Z)
{
	for (int i = 0; i<6; ++i)
	{
		for (int j = 0; j<640*480; ++j)
		L_Z_trans[i][j] = L_Z[j][i];
	}
	return L_Z_trans;
}


Eigen::MatrixXd getL_Z_trans_Eigen(Eigen::MatrixXd L_Z_trans_Eigen, double** L_Z_trans)
{
	L_Z_trans_Eigen = Eigen::MatrixXd::Zero(6,640*480);
//	Eigen::MatrixXd L_Z_trans_Eigen2 = Eigen::MatrixXd::Zero(640*480,6);
//	L_Z_trans_Eigen = L_Z_trans_Eigen2.transpose();
	for (int i = 0; i<6; ++i)
	{
		for (int j = 0; j<640*480; ++j)
		L_Z_trans_Eigen(i,j) = L_Z_trans[i][j];
	}
	return L_Z_trans_Eigen;
}


double** getL_Z_trans_L_Z(double** L_Z_trans_L_Z, double** L_Z_trans, double** L_Z)
{
	double temporary_value = 0;
	for (int i = 0; i<6; ++i)
	{
		for (int j = 0; j<6; ++j)
		{
			temporary_value = 0;
			for (int k = 0; k<640*480; ++k)
			{
				temporary_value = temporary_value + L_Z_trans[i][k] * L_Z[k][j];
				L_Z_trans_L_Z[i][j] = temporary_value;
			}
				
		}
	}
	return L_Z_trans_L_Z;
}


Eigen::MatrixXd getL_Z_trans_L_Z_Eigen(Eigen::MatrixXd L_Z_trans_L_Z_Eigen, double** L_Z_trans_L_Z)
{
	for (int i = 0; i<6; ++i)
	{
		for (int j = 0; j<6; ++j)
		L_Z_trans_L_Z_Eigen(i,j) = L_Z_trans_L_Z[i][j];
	}
	return L_Z_trans_L_Z_Eigen;
}


Eigen::MatrixXd getL_Z_plus_Eigen(Eigen::MatrixXd L_Z_plus_Eigen ,Eigen::MatrixXd L_Z_trans_L_Z_Eigen, Eigen::MatrixXd L_Z_trans_Eigen)
{
	L_Z_plus_Eigen = L_Z_trans_L_Z_Eigen.inverse()*L_Z_trans_Eigen;
	return L_Z_plus_Eigen;
}


Eigen::MatrixXd getInteractionMatrix(USHORT* depth2, double* L_x, double* L_y, double* L_P_Z, double dZ_dx, double dZ_dy, double** L_Z, double** L_Z_trans, Eigen::MatrixXd L_Z_trans_Eigen, double** L_Z_trans_L_Z, Eigen::MatrixXd L_Z_trans_L_Z_Eigen, Eigen::MatrixXd L_Z_plus_Eigen)
{
	L_Z = getL_Z(depth2, L_x, L_y, L_P_Z, dZ_dx, dZ_dy, L_Z);

	L_Z_trans = getL_Z_trans(L_Z_trans, L_Z);

	L_Z_trans_Eigen = getL_Z_trans_Eigen(L_Z_trans_Eigen, L_Z_trans);
	
	L_Z_trans_L_Z = getL_Z_trans_L_Z(L_Z_trans_L_Z, L_Z_trans, L_Z);
	
	L_Z_trans_L_Z_Eigen = getL_Z_trans_L_Z_Eigen(L_Z_trans_L_Z_Eigen, L_Z_trans_L_Z);
	
	L_Z_plus_Eigen = getL_Z_plus_Eigen(L_Z_plus_Eigen, L_Z_trans_L_Z_Eigen, L_Z_trans_Eigen);

	return L_Z_plus_Eigen;
}