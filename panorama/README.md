## Panorama

### 一、  算法步骤
1.  对每个图像进行柱状投影
2.  针对每个图像，提取其SIFT特征（用灰度图像能加快速度）
3.  用kd树为每个特征找到k个最近邻
4.  对于每幅图：
 - 选取m个候选的匹配图
 -  用RANSAC去计算出每个图像对之间的齐次矩阵
 -   用概率模型验证这个图像匹配
5.  找到图像匹配之间的matching point
6.  对每个相连的部件：
 -  计算出移动的x和y
 -  用multi-band blending的方法拼接起来
### 二、  算法描述
1.  投影

 - 在做全景拼接的时候，为了保持图片中的空间约束与视觉的一致性，需要做一定的预处理，可以是球面投影，也可以是柱面投影。本次作业是做水平方向的拼接，因此，完成柱面投影即可。
参考博客 [【Octave】柱面投影简析](https://www.cnblogs.com/cheermyang/p/5431170.html)

2.  调用[Vlfeat库](http://www.vlfeat.org/overview/sift.)html，提取其SIFT特征，并使用Vlfeat库中的kdtree为每个特征找到[k个最近邻](http://www.vlfeat.org/overview/kdtree.html)
3.  RANSAC算法

 -  随机选择四个特征对
 -  计算出他们的齐次矩阵
 -   计算SSD(pi’, H*pi) < e的inliers
 -  记录当前的最大inliers集合
 -  重复上述步骤k次(k根据置信概率和outliers ratio计算得出)
 -  重新计算上述计算出的最大inliers集合的least-square H。
4.  计算齐次矩阵Homography Matrix
 - 根据给出的四个顶点对，套用 Ref4的算法，计算出M，即为齐次矩阵

5.  Blending拼接
 - 我拼接图像的做法是从RANSAC中得到的inliners对中学习它的平均x, y位移大小，然后用该位移大小计算出blending时的min_x和min_y，并根据以上的数据进行拼接。Ref5中的Multi-bend
