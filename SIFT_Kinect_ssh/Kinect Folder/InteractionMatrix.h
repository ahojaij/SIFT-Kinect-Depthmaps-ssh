#include <d2d1.h>
#include <d2d1helper.h>
#include <Eigen/LU>
#include <Eigen/Core>
#include <Eigen/src/Core/Transpose.h>

double**                    getL_Z(USHORT* , double* , double* , double* , double , double, double**);

double**                    getL_Z_trans(double**, double**);

Eigen::MatrixXd             getL_Z_trans_Eigen(Eigen::MatrixXd, double**);

double**                    getL_Z_trans_L_Z(double**, double**, double**);

Eigen::MatrixXd             getL_Z_trans_L_Z_Eigen(Eigen::MatrixXd, double**);

Eigen::MatrixXd             getL_Z_plus_Eigen(Eigen::MatrixXd, Eigen::MatrixXd , Eigen::MatrixXd);

Eigen::MatrixXd             getInteractionMatrix(USHORT* , double* , double* , double* , double , double , double** , double** , Eigen::MatrixXd , double** , Eigen::MatrixXd , Eigen::MatrixXd);

