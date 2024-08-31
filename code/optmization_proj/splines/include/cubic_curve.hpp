#ifndef CUBIC_CURVE_HPP
#define CUBIC_CURVE_HPP

#include <Eigen/Eigen>

#include <iostream>
#include <cmath>
#include <cfloat>
#include <vector>

//单段2维3次多项式
class CubicPolynomial
{
private:
    //多项式的时间长度
    double duration;
    //多项式的系数矩阵，大小是2x4，2行代表维度是2维，第1到第4列分别代表t^3,t^2,t和1的系数向量
    Eigen::Matrix<double, 2, 4> coeffMat;

public:
    CubicPolynomial() = default;

    //直接从时长和系数矩阵中构造一个单段3次多项式
    CubicPolynomial(double dur, const Eigen::Matrix<double, 2, 4> &cMat)
        : duration(dur), coeffMat(cMat) {}

    //获取维度信息
    inline int getDim() const
    {
        return 2;
    }

    //获取多项式最高次数
    inline int getDegree() const
    {
        return 3;
    }

    //获取时间长度
    inline double getDuration() const
    {
        return duration;
    }

    //获取系数矩阵
    inline const Eigen::Matrix<double, 2, 4> &getCoeffMat() const
    {
        return coeffMat;
    }

    //给定0到duration范围内的时间，获取对应的2维位置
    inline Eigen::Vector2d getPos(const double &t) const
    {
        return coeffMat.col(3) + t * (coeffMat.col(2) + t * (coeffMat.col(1) + t * coeffMat.col(0)));
    }
};

//多段2维3次多项式构成的曲线
class CubicCurve
{
private:
    //多段等价于单段2维3次多项式构成的向量
    typedef std::vector<CubicPolynomial> Pieces;
    Pieces pieces;

public:
    CubicCurve() = default;

    //从时间向量和系数向量中构造多段3次多项式曲线，两个向量长度必须一致才合法
    CubicCurve(const std::vector<double> &durs,
               const std::vector<Eigen::Matrix<double, 2, 4>> &cMats)
    {
        const int N = std::min(durs.size(), cMats.size());
        pieces.reserve(N);
        for (int i = 0; i < N; ++i)
        {
            pieces.emplace_back(durs[i], cMats[i]);
        }
    }

    //获取当前多段3次多项式曲线的段数
    inline int getPieceNum() const
    {
        return pieces.size();
    }

    //获取当前多段3次多项式曲线的时间向量
    inline Eigen::VectorXd getDurations() const
    {
        const int N = getPieceNum();
        Eigen::VectorXd durations(N);
        for (int i = 0; i < N; ++i)
        {
            durations(i) = pieces[i].getDuration();
        }
        return durations;
    }

    //获取当前多段3次多项式曲线的总时间，即各个段的时间之和
    inline double getTotalDuration() const
    {
        const int N = getPieceNum();
        double totalDuration = 0.0;
        for (int i = 0; i < N; ++i)
        {
            totalDuration += pieces[i].getDuration();
        }
        return totalDuration;
    }

    //获取由每一段开头位置和最后一段末尾位置构成的位置矩阵，大小为2x(N+1)
    inline Eigen::Matrix2Xd getPositions() const
    {
        const int N = getPieceNum();
        Eigen::Matrix2Xd positions(2, N + 1);
        for (int i = 0; i < N; ++i)
        {
            positions.col(i) = pieces[i].getCoeffMat().col(3);
        }
        positions.col(N) = pieces[N - 1].getPos(pieces[N - 1].getDuration());
        return positions;
    }

    //获取第i段的2维3次多项式的常量引用
    inline const CubicPolynomial &operator[](int i) const
    {
        return pieces[i];
    }

    //获取第i段的2维3次多项式的mutable引用
    inline CubicPolynomial &operator[](int i)
    {
        return pieces[i];
    }

    //清空曲线的所有段
    inline void clear(void)
    {
        pieces.clear();
        return;
    }

    //获取初始常量迭代器
    inline Pieces::const_iterator begin() const
    {
        return pieces.begin();
    }

    //获取终止常量迭代器
    inline Pieces::const_iterator end() const
    {
        return pieces.end();
    }

    //获取初始迭代器
    inline Pieces::iterator begin()
    {
        return pieces.begin();
    }

    //获取终止迭代器
    inline Pieces::iterator end()
    {
        return pieces.end();
    }

    //预存空间
    inline void reserve(const int &n)
    {
        pieces.reserve(n);
        return;
    }

    //在末尾添加一个新的段
    inline void emplace_back(const CubicPolynomial &piece)
    {
        pieces.emplace_back(piece);
        return;
    }

    //在末尾利用时间长度和系数矩阵构造一个新的段
    inline void emplace_back(const double &dur,
                             const Eigen::Matrix<double, 2, 4> &cMat)
    {
        pieces.emplace_back(dur, cMat);
        return;
    }

    //在末尾拼接一个新的多段3次多项式曲线
    inline void append(const CubicCurve &traj)
    {
        pieces.insert(pieces.end(), traj.begin(), traj.end());
        return;
    }

    //给定相对第一段开始的时间，获取其所在的段数
    inline int locatePieceIdx(double &t) const
    {
        const int N = getPieceNum();
        int idx;
        double dur;
        for (idx = 0;
             idx < N &&
             t > (dur = pieces[idx].getDuration());
             idx++)
        {
            t -= dur;
        }
        if (idx == N)
        {
            idx--;
            t += pieces[idx].getDuration();
        }
        return idx;
    }

    //给定相对第一段开始的时间，获取其多段3次多项式曲线对应的2维位置
    inline Eigen::Vector2d getPos(double t) const
    {
        const int pieceIdx = locatePieceIdx(t);
        return pieces[pieceIdx].getPos(t);
    }

    //获取第juncIdx个节点位置，所谓节点就是每一段的开头或者最后一段末尾
    inline Eigen::Vector2d getJuncPos(const int juncIdx) const
    {
        if (juncIdx != getPieceNum())
        {
            return pieces[juncIdx].getCoeffMat().col(3);
        }
        else
        {
            return pieces[juncIdx - 1].getPos(pieces[juncIdx - 1].getDuration());
        }
    }
};

#endif
