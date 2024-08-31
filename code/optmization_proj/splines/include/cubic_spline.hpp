#ifndef CUBIC_SPLINE_HPP
#define CUBIC_SPLINE_HPP

#include "cubic_curve.hpp"

#include <Eigen/Eigen>

#include <cmath>
#include <vector>

namespace cubic_spline
{

    // The banded system class is used for solving
    // banded linear system Ax=b efficiently.
    // A is an N*N band matrix with lower band width lowerBw
    // and upper band width upperBw.
    // Banded LU factorization has O(N) time complexity.
    class BandedSystem
    {
    public:
        // The size of A, as well as the lower/upper
        // banded width p/q are needed
        inline void create(const int &n, const int &p, const int &q)
        {
            // In case of re-creating before destroying
            destroy();
            N = n;
            lowerBw = p;
            upperBw = q;
            int actualSize = N * (lowerBw + upperBw + 1);
            ptrData = new double[actualSize];
            std::fill_n(ptrData, actualSize, 0.0);
            return;
        }

        inline void destroy()
        {
            if (ptrData != nullptr)
            {
                delete[] ptrData;
                ptrData = nullptr;
            }
            return;
        }

    private:
        int N;
        int lowerBw;
        int upperBw;
        // Compulsory nullptr initialization here
        double *ptrData = nullptr;

    public:
        // Reset the matrix to zero
        inline void reset(void)
        {
            std::fill_n(ptrData, N * (lowerBw + upperBw + 1), 0.0);
            return;
        }

        // The band matrix is stored as suggested in "Matrix Computation"
        inline const double &operator()(const int &i, const int &j) const
        {
            return ptrData[(i - j + upperBw) * N + j];
        }

        inline double &operator()(const int &i, const int &j)
        {
            return ptrData[(i - j + upperBw) * N + j];
        }

        // This function conducts banded LU factorization in place
        // Note that NO PIVOT is applied on the matrix "A" for efficiency!!!
        inline void factorizeLU()
        {
            int iM, jM;
            double cVl;
            for (int k = 0; k <= N - 2; ++k)
            {
                iM = std::min(k + lowerBw, N - 1);
                cVl = operator()(k, k);
                for (int i = k + 1; i <= iM; ++i)
                {
                    if (operator()(i, k) != 0.0)
                    {
                        operator()(i, k) /= cVl;
                    }
                }
                jM = std::min(k + upperBw, N - 1);
                for (int j = k + 1; j <= jM; ++j)
                {
                    cVl = operator()(k, j);
                    if (cVl != 0.0)
                    {
                        for (int i = k + 1; i <= iM; ++i)
                        {
                            if (operator()(i, k) != 0.0)
                            {
                                operator()(i, j) -= operator()(i, k) * cVl;
                            }
                        }
                    }
                }
            }
            return;
        }

        // This function solves Ax=b, then stores x in b
        // The input b is required to be N*m, i.e.,
        // m vectors to be solved.
        template <typename EIGENMAT>
        inline void solve(EIGENMAT &b) const
        {
            int iM;
            for (int j = 0; j <= N - 1; ++j)
            {
                iM = std::min(j + lowerBw, N - 1);
                for (int i = j + 1; i <= iM; ++i)
                {
                    if (operator()(i, j) != 0.0)
                    {
                        b.row(i) -= operator()(i, j) * b.row(j);
                    }
                }
            }
            for (int j = N - 1; j >= 0; --j)
            {
                b.row(j) /= operator()(j, j);
                iM = std::max(0, j - upperBw);
                for (int i = iM; i <= j - 1; ++i)
                {
                    if (operator()(i, j) != 0.0)
                    {
                        b.row(i) -= operator()(i, j) * b.row(j);
                    }
                }
            }
            return;
        }

        // This function solves ATx=b, then stores x in b
        // The input b is required to be N*m, i.e.,
        // m vectors to be solved.
        template <typename EIGENMAT>
        inline void solveAdj(EIGENMAT &b) const
        {
            int iM;
            for (int j = 0; j <= N - 1; ++j)
            {
                b.row(j) /= operator()(j, j);
                iM = std::min(j + upperBw, N - 1);
                for (int i = j + 1; i <= iM; ++i)
                {
                    if (operator()(j, i) != 0.0)
                    {
                        b.row(i) -= operator()(j, i) * b.row(j);
                    }
                }
            }
            for (int j = N - 1; j >= 0; --j)
            {
                iM = std::max(0, j - lowerBw);
                for (int i = iM; i <= j - 1; ++i)
                {
                    if (operator()(j, i) != 0.0)
                    {
                        b.row(i) -= operator()(j, i) * b.row(j);
                    }
                }
            }
            return;
        }
    };

    //每段时间长度均为1的三次样条
    class CubicSpline
    {
    public:
        CubicSpline() = default;
        ~CubicSpline() { A.destroy(); }

    private:
        int N;
        Eigen::Vector2d headP;
        Eigen::Vector2d tailP;
        BandedSystem A;
        Eigen::MatrixX2d b;

    public:
        inline void setConditions(const Eigen::Vector2d &headPos,
                                  const Eigen::Vector2d &tailPos,
                                  const int &pieceNum)
        {
            //这里直接用高阶连续性的条件来计算三次样条
            //即课件中所讲的三次样条和给定位置条件并令中间位置点处的位置、速度和加速度均连续的三次样条曲线理论上完全等价
            N = pieceNum;
            headP = headPos;
            tailP = tailPos;
            //直接求解系数矩阵，那每一段都是4个系数，因此A矩阵是边长为4N的方阵
            //相邻两个段之间的位置、速度和加速度均连续加上位置给定，因此列出来会发现上下带宽都是4
            A.create(4 * N, 4, 4);
            //所考虑的空间维度是2，因此系数向量是2维，因此常数矩阵b大小是4N*2
            b.resize(4 * N, 2);

            // This is minimum stretch energy cubic spline,
            // but the matrix is different from the one mentioned 
            // in the course. This is because a theoretically 
            // equivalent formulation is applied here. All kinds of 
            // minimum energy spline share one kind of perfect 
            // description by high orders of continuity. If you 
            // are interested, see Theorem 2 in the paper named
            // "Geometrically Constrained Trajectory Optimization for Multicopters"
            // Moreover, further decouple of DoFs (number of waypoints) and 
            // constraint resolution can be done. However, here we use 
            // a somewhat inefficient form of dense waypoints such that 
            // the homework is much easier.
            A.reset();
            //开始为矩阵A赋值并提前做好分解，b暂时不在本函数里赋值
            //Ax=b里假设x的第4i行到4i+3行依次是第i段的1,t,t^2,t^3的2维系数列向量的转置
            A(0, 0) = 1.0; //第一段开始位置给定，给定值在b里面，A里只放系数
            A(1, 1) = 1.0; //第一段开始速度给定为0，给定值在b里面，A里只放系数
            for (int i = 0; i < N - 1; ++i)
            {
                //注意此处的顺序不能变更，首先是加速度相等、再是位置给定、再是位置相等、最后是速度相等
                //之所以要提前固定这个顺序，是因为理论上可证明这种顺序在带状分解时不需要pivot，速度更快。
                //若采用课件中的带状矩阵来计算部分系数，不一次性整体计算所有系数矩阵，
                //也可以直接进行不带pivot的LU分解，因为课件中的办法构成的带状矩阵是严格对角占优的。

                //第一段末尾t=1加速度和第二段开始t=0加速度相等，即它们差值为0，差值在b里面，A里只放系数
                A(4 * i + 2, 4 * i + 2) = 2.0;
                A(4 * i + 2, 4 * i + 3) = 6.0;
                A(4 * i + 2, 4 * i + 6) = -2.0;
                //第一段末尾t=1的位置给定，给定值在b里面，A里只放系数
                A(4 * i + 3, 4 * i) = 1.0;
                A(4 * i + 3, 4 * i + 1) = 1.0;
                A(4 * i + 3, 4 * i + 2) = 1.0;
                A(4 * i + 3, 4 * i + 3) = 1.0;
                //第一段末尾t=1位置和第二段开始t=0位置相等，即它们差值为0，差值在b里面，A里只放系数
                A(4 * i + 4, 4 * i) = 1.0;
                A(4 * i + 4, 4 * i + 1) = 1.0;
                A(4 * i + 4, 4 * i + 2) = 1.0;
                A(4 * i + 4, 4 * i + 3) = 1.0;
                A(4 * i + 4, 4 * i + 4) = -1.0;
                //第一段末尾t=1速度和第二段开始t=0速度相等，即它们差值为0，差值在b里面，A里只放系数
                A(4 * i + 5, 4 * i + 1) = 1.0;
                A(4 * i + 5, 4 * i + 2) = 2.0;
                A(4 * i + 5, 4 * i + 3) = 3.0;
                A(4 * i + 5, 4 * i + 5) = -1.0;
            }
            //最后一段末尾t=1的位置给定，给定值在b里面，A里只放系数
            A(4 * N - 2, 4 * N - 4) = 1.0;
            A(4 * N - 2, 4 * N - 3) = 1.0;
            A(4 * N - 2, 4 * N - 2) = 1.0;
            A(4 * N - 2, 4 * N - 1) = 1.0;
            //最后一段末尾t=1的速度给定为0，给定值在b里面，A里只放系数
            A(4 * N - 1, 4 * N - 3) = 1.0;
            A(4 * N - 1, 4 * N - 2) = 2.0;
            A(4 * N - 1, 4 * N - 1) = 3.0;
            A.factorizeLU();

            return;
        }

        //设定b矩阵中的位置向量，即除开头和末尾处剩余中心节点的位置，然后进行系数向量求解并存在b里面
        //调用后b被原地更改了，其储存的值就是x，其第4i行到4i+3行依次是1,t,t^2,t^3的2维系数列向量的转置
        inline void setInnerPoints(const Eigen::Ref<const Eigen::Matrix2Xd> &inPs)
        {
            b.setZero();
            b.row(0) = headP.transpose();
            for (int i = 0; i < N - 1; ++i)
            {
                b.row(4 * i + 3) = inPs.col(i).transpose();
            }
            b.row(4 * N - 2) = tailP.transpose();

            A.solve(b);

            return;
        }

        //获取计算得到的多段2维3次多项式构成的曲线，注意此处求解得到的曲线满足三次样条的各种连续性条件，需要在setInnerPoints之后调用
        inline void getCurve(CubicCurve &curve) const
        {
            curve.clear();
            curve.reserve(N);
            for (int i = 0; i < N; ++i)
            {
                curve.emplace_back(1.0,
                                   b.block<4, 2>(4 * i, 0)
                                       .transpose()
                                       .rowwise()
                                       .reverse());
            }
            return;
        }

        //获取计算得到的多段2维3次多项式构成的曲线的能量J，即加速度平方积分，需要在setInnerPoints之后调用
        inline void getStretchEnergy(double &energy) const
        {
            energy = 0.0;
            for (int i = 0; i < N; ++i)
            {
                energy += 4.0 * b.row(4 * i + 2).squaredNorm() +
                          12.0 * b.row(4 * i + 2).dot(b.row(4 * i + 3)) +
                          12.0 * b.row(4 * i + 3).squaredNorm();
            }
            return;
        }

        //获取计算得到的多段2维3次多项式构成的曲线的系数矩阵向量，需要在setInnerPoints之后调用
        inline const Eigen::MatrixX2d &getCoeffs(void) const
        {
            return b;
        }

        //获取能量J关于三次样条除开头和末尾之外中心节点的位置向量的梯度，大小为2*(N-1)，需要在setInnerPoints之后调用
        inline void getGrad(Eigen::Ref<Eigen::Matrix2Xd> gradByPoints) const
        {
            Eigen::MatrixX2d gdC(4 * N, 2);
            for (int i = 0; i < N; ++i)
            {
                gdC.row(4 * i + 3) = 12.0 * b.row(4 * i + 2) +
                                     24.0 * b.row(4 * i + 3);
                gdC.row(4 * i + 2) = 8.0 * b.row(4 * i + 2) +
                                     12.0 * b.row(4 * i + 3);
                gdC.block<2, 2>(4 * i, 0).setZero();
            }

            A.solveAdj(gdC);

            gradByPoints.resize(2, N - 1);
            for (int i = 0; i < N - 1; ++i)
            {
                gradByPoints.col(i) = gdC.row(4 * i + 3).transpose();
            }
        }
    };
}

#endif
