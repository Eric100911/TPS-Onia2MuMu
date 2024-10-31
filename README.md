## 整体介绍

在CMS实验上测量质子-质子对撞中一类三介子联合产生过程的反应截面。

这一反应截面部分由三部分子散射（Triple Parton Scattering, TPS）贡献，因而可用于研究质子中部分子动力学分布的关联，并且能为未来超出标准模型物理的搜索提供更为精确的本底描述。

本软件包将用于从`MINIAOD`原始数据中初步进行筛选，将候选事例汇总并存储相关信息，以备后续进一步筛选。

## 反应过程

$pp \rightarrow J/\psi + J/\psi + \Upsilon \rightarrow 6\mu$

$pp \rightarrow J/\psi + J/\psi + \phi \rightarrow 4\mu + 2K$

$pp \rightarrow J/\psi + \Upsilon + \phi \rightarrow 4\mu + 2K$

## 软件基础

使用CERN的`ROOT`框架，以及CMS合作组软件框架`CMSSW_13_0_2`。

从github.com/AliceQuen/Onia2MuMu继承了代码框架。相关代码由清华大学致理书院2021级本科生秦俊凯提供。

## 处理方法总览

### 事件选择

#### 1. 将$\mu^+\mu^-$配对：仅处理几何条件

从轨迹拟合顶点，检查是否能够给出一定结果，不限制具体`VtxProb`。

#### 2. 将$K^+K^-$配对：仅处理几何条件

从带电径迹中排除$\mu^{\pm}$， 拟合顶点，检查是否能够给出一定结果，不限制具体`VtxProb`。

#### 2. 从$\mu^+\mu^-$对创建候选$J/\psi$和$\Upsilon$：使用部分动力学信息
从通过上一步筛选的$\mu^+\mu^-$对出发，通过不变质量进行分选：

* 视为$J/\psi$候选：满足 $ 2 \mathrm{GeV/c^2} \leq m_{\mu^+\mu^-} \leq 6 \mathrm{GeV/c^2}$

* 视为$\Upsilon$候选：满足 $ 8 \mathrm{GeV/c^2} \leq m_{\mu^+\mu^-} \leq 12 \mathrm{GeV/c^2}$

#### 3. 从带电径迹对创建候选$\phi$：使用部分动力学信息





随后使用不变质量进行初步筛选，要求 $ 0.5 \mathrm{GeV/c^2} \leq m_{K^+K^-} \leq 1.5 \mathrm{GeV/c^2}$。


#### 3. 从候选$J/\psi$和$\Upsilon$寻找整体事例候选

仅处理几何条件：从上一步筛选的$J/\psi$和$\Upsilon$出发，通过顶点拟合，寻找整体事例候选。

具体过程：选择两个候选$J/\psi$和一个候选$\Upsilon$，在确认所使用的$\mu^{\pm}$不相重合后，通过顶点拟合，寻找一个共同的顶点。

### 效率评定

（待完善，或另起repository进行）

### 系统误差

（待完善，或另起repository进行）

## 主要贡献者

* 王驰(Eric100911)：清华大学致理书院2022级本科生
* 程幸(Endymion)：清华大学致理书院2022级本科生
* 石镇鹏：清华大学物理系2023级本科生

指导教师：胡震副教授

## 数据集

### Run 3：主要数据来源

本次分析将主要使用Run 3的数据，利用较大数据量寻找以上所列各个相对稀有的过程。

结合本课题目标过程，选用Run 3中2022以及2023年`ParkingDoubleMuonLowMass[1-7]`系列数据集。

#### 关于Run2023C的特别提示
在Run2023C数据采集阶段，曾在RAW与AOD之间出现过一些数据处理问题。

这个问题的影响是，在提交crab job时，**需要特别指定Run2023C-v2这一部分数据的run range为$[367516, 367620]$**。

具体信息可以参照[Run3 PdmV twiki](https://twiki.cern.ch/twiki/bin/viewauth/CMS/PdmVRun3Analysis#2023_Era_definition)，以及相关[CMS Talk post](https://cms-talk.web.cern.ch/t/t0-reprocessing-due-to-wrong-raw-version/24528)和[工单](https://its.cern.ch/jira/browse/CMSTZ-1040)

### Run 2：辅助确认

在编写得到$pp \rightarrow J/\psi + J/\psi + \phi \rightarrow 4\mu + 2K$代码之后，将会使用Run 2数据进行分析，用于复现日内瓦大学S. Leontsinis等人结果；通过交叉比对，确认代码可靠性。


### 数据查询工具：CMS DAS命令行界面

运行前需要设置好VOMS代理。调用时，需要使用语句：

```
/cvmfs/cms.cern.ch/common/dasgoclient -query="YOUR_DAS_QUERY"
```

其中，查询语句`YOUR_DAS_QUERY`与在网页上查看CMS DAS无异。具体使用可以参考https://cmsweb.cern.ch/das/cli 。

## 附录：数据集标签

#### Run 2

这部分与S. Leontsinis等人的报告一致。

##### 2016
```
/Charmonium/Run2016B-21Feb2020_ver1_UL2016_HIPM-v1/MINIAOD
/Charmonium/Run2016B-21Feb2020_ver2_UL2016_HIPM-v1/MINIAOD
/Charmonium/Run2016C-21Feb2020_UL2016_HIPM-v1/MINIAOD
/Charmonium/Run2016D-21Feb2020_UL2016_HIPM-v1/MINIAOD
/Charmonium/Run2016E-21Feb2020_UL2016_HIPM-v1/MINIAOD
/Charmonium/Run2016F-21Feb2020_UL2016_HIPM-v1/MINIAOD
/Charmonium/Run2016G-21Feb2020_UL2016-v1/MINIAOD
```

##### 2017

```
/Charmonium/Run2017B-UL2017_MiniAODv2-v1/MINIAOD
/Charmonium/Run2017C-UL2017_MiniAODv2-v1/MINIAOD
/Charmonium/Run2017D-UL2017_MiniAODv2-v1/MINIAOD
/Charmonium/Run2017E-UL2017_MiniAODv2-v1/MINIAOD
/Charmonium/Run2017F-UL2017_MiniAODv2-v1/MINIAOD
```

##### 2018

```
/Charmonium/Run2018A-12Nov2019_UL2018_rsb-v1/MINIAOD
/Charmonium/Run2018B-UL2018_MiniAODv2-v1/MINIAOD
/Charmonium/Run2018C-UL2018_MiniAODv2-v1/MINIAOD
/Charmonium/Run2018D-12Nov2019_UL2018-v1/MINIAOD
```
#### Run 3

这部分综合了`test/crabData/Run2023dataList.txt`（cr. 王晰宁）以及PdmV查询结果。

##### 2022 （需使用`CMSSW_12_4_x`）

```
/ParkingDoubleMuonLowMass0/Run2022C-PromptReco-v1/MINIAOD
/ParkingDoubleMuonLowMass0/Run2022D-PromptReco-v1/MINIAOD
/ParkingDoubleMuonLowMass0/Run2022D-PromptReco-v2/MINIAOD
/ParkingDoubleMuonLowMass0/Run2022E-PromptReco-v1/MINIAOD
/ParkingDoubleMuonLowMass0/Run2022F-PromptReco-v1/MINIAOD
/ParkingDoubleMuonLowMass0/Run2022G-PromptReco-v1/MINIAOD
/ParkingDoubleMuonLowMass1/Run2022C-PromptReco-v1/MINIAOD
/ParkingDoubleMuonLowMass1/Run2022D-PromptReco-v1/MINIAOD
/ParkingDoubleMuonLowMass1/Run2022D-PromptReco-v2/MINIAOD
/ParkingDoubleMuonLowMass1/Run2022E-PromptReco-v1/MINIAOD
/ParkingDoubleMuonLowMass1/Run2022F-PromptReco-v1/MINIAOD
/ParkingDoubleMuonLowMass1/Run2022G-PromptReco-v1/MINIAOD
/ParkingDoubleMuonLowMass2/Run2022C-PromptReco-v1/MINIAOD
/ParkingDoubleMuonLowMass2/Run2022D-PromptReco-v1/MINIAOD
/ParkingDoubleMuonLowMass2/Run2022D-PromptReco-v2/MINIAOD
/ParkingDoubleMuonLowMass2/Run2022E-PromptReco-v1/MINIAOD
/ParkingDoubleMuonLowMass2/Run2022F-PromptReco-v1/MINIAOD
/ParkingDoubleMuonLowMass2/Run2022G-PromptReco-v1/MINIAOD
/ParkingDoubleMuonLowMass3/Run2022C-PromptReco-v1/MINIAOD
/ParkingDoubleMuonLowMass3/Run2022D-PromptReco-v1/MINIAOD
/ParkingDoubleMuonLowMass3/Run2022D-PromptReco-v2/MINIAOD
/ParkingDoubleMuonLowMass3/Run2022E-PromptReco-v1/MINIAOD
/ParkingDoubleMuonLowMass3/Run2022F-PromptReco-v1/MINIAOD
/ParkingDoubleMuonLowMass3/Run2022G-PromptReco-v1/MINIAOD
/ParkingDoubleMuonLowMass4/Run2022C-PromptReco-v1/MINIAOD
/ParkingDoubleMuonLowMass4/Run2022D-PromptReco-v1/MINIAOD
/ParkingDoubleMuonLowMass4/Run2022D-PromptReco-v2/MINIAOD
/ParkingDoubleMuonLowMass4/Run2022E-PromptReco-v1/MINIAOD
/ParkingDoubleMuonLowMass4/Run2022F-PromptReco-v1/MINIAOD
/ParkingDoubleMuonLowMass4/Run2022G-PromptReco-v1/MINIAOD
/ParkingDoubleMuonLowMass5/Run2022C-PromptReco-v1/MINIAOD
/ParkingDoubleMuonLowMass5/Run2022D-PromptReco-v1/MINIAOD
/ParkingDoubleMuonLowMass5/Run2022D-PromptReco-v2/MINIAOD
/ParkingDoubleMuonLowMass5/Run2022E-PromptReco-v1/MINIAOD
/ParkingDoubleMuonLowMass5/Run2022F-PromptReco-v1/MINIAOD
/ParkingDoubleMuonLowMass5/Run2022G-PromptReco-v1/MINIAOD
/ParkingDoubleMuonLowMass6/Run2022C-PromptReco-v1/MINIAOD
/ParkingDoubleMuonLowMass6/Run2022D-PromptReco-v1/MINIAOD
/ParkingDoubleMuonLowMass6/Run2022D-PromptReco-v2/MINIAOD
/ParkingDoubleMuonLowMass6/Run2022E-PromptReco-v1/MINIAOD
/ParkingDoubleMuonLowMass6/Run2022F-PromptReco-v1/MINIAOD
/ParkingDoubleMuonLowMass6/Run2022G-PromptReco-v1/MINIAOD
/ParkingDoubleMuonLowMass7/Run2022C-PromptReco-v1/MINIAOD
/ParkingDoubleMuonLowMass7/Run2022D-PromptReco-v1/MINIAOD
/ParkingDoubleMuonLowMass7/Run2022D-PromptReco-v2/MINIAOD
/ParkingDoubleMuonLowMass7/Run2022E-PromptReco-v1/MINIAOD
/ParkingDoubleMuonLowMass7/Run2022F-PromptReco-v1/MINIAOD
/ParkingDoubleMuonLowMass7/Run2022G-PromptReco-v1/MINIAOD
```

##### 2023 （需使用`CMSSW_13_0_x`）

```
/ParkingDoubleMuonLowMass0/Run2023B-PromptReco-v1/MINIAOD
/ParkingDoubleMuonLowMass0/Run2023C-PromptReco-v1/MINIAOD
/ParkingDoubleMuonLowMass0/Run2023C-PromptReco-v2/MINIAOD
/ParkingDoubleMuonLowMass0/Run2023C-PromptReco-v3/MINIAOD
/ParkingDoubleMuonLowMass0/Run2023C-PromptReco-v4/MINIAOD
/ParkingDoubleMuonLowMass0/Run2023D-PromptReco-v1/MINIAOD
/ParkingDoubleMuonLowMass0/Run2023D-PromptReco-v2/MINIAOD
/ParkingDoubleMuonLowMass1/Run2023B-PromptReco-v1/MINIAOD
/ParkingDoubleMuonLowMass1/Run2023C-PromptReco-v1/MINIAOD
/ParkingDoubleMuonLowMass1/Run2023C-PromptReco-v2/MINIAOD
/ParkingDoubleMuonLowMass1/Run2023C-PromptReco-v3/MINIAOD
/ParkingDoubleMuonLowMass1/Run2023C-PromptReco-v4/MINIAOD
/ParkingDoubleMuonLowMass1/Run2023D-PromptReco-v1/MINIAOD
/ParkingDoubleMuonLowMass1/Run2023D-PromptReco-v2/MINIAOD
/ParkingDoubleMuonLowMass2/Run2023B-PromptReco-v1/MINIAOD
/ParkingDoubleMuonLowMass2/Run2023C-PromptReco-v1/MINIAOD
/ParkingDoubleMuonLowMass2/Run2023C-PromptReco-v2/MINIAOD
/ParkingDoubleMuonLowMass2/Run2023C-PromptReco-v3/MINIAOD
/ParkingDoubleMuonLowMass2/Run2023C-PromptReco-v4/MINIAOD
/ParkingDoubleMuonLowMass2/Run2023D-PromptReco-v1/MINIAOD
/ParkingDoubleMuonLowMass2/Run2023D-PromptReco-v2/MINIAOD
/ParkingDoubleMuonLowMass3/Run2023B-PromptReco-v1/MINIAOD
/ParkingDoubleMuonLowMass3/Run2023C-PromptReco-v1/MINIAOD
/ParkingDoubleMuonLowMass3/Run2023C-PromptReco-v2/MINIAOD
/ParkingDoubleMuonLowMass3/Run2023C-PromptReco-v3/MINIAOD
/ParkingDoubleMuonLowMass3/Run2023C-PromptReco-v4/MINIAOD
/ParkingDoubleMuonLowMass3/Run2023D-PromptReco-v1/MINIAOD
/ParkingDoubleMuonLowMass3/Run2023D-PromptReco-v2/MINIAOD
/ParkingDoubleMuonLowMass4/Run2023B-PromptReco-v1/MINIAOD
/ParkingDoubleMuonLowMass4/Run2023C-PromptReco-v1/MINIAOD
/ParkingDoubleMuonLowMass4/Run2023C-PromptReco-v2/MINIAOD
/ParkingDoubleMuonLowMass4/Run2023C-PromptReco-v3/MINIAOD
/ParkingDoubleMuonLowMass4/Run2023C-PromptReco-v4/MINIAOD
/ParkingDoubleMuonLowMass4/Run2023D-PromptReco-v1/MINIAOD
/ParkingDoubleMuonLowMass4/Run2023D-PromptReco-v2/MINIAOD
/ParkingDoubleMuonLowMass5/Run2023B-PromptReco-v1/MINIAOD
/ParkingDoubleMuonLowMass5/Run2023C-PromptReco-v1/MINIAOD
/ParkingDoubleMuonLowMass5/Run2023C-PromptReco-v2/MINIAOD
/ParkingDoubleMuonLowMass5/Run2023C-PromptReco-v3/MINIAOD
/ParkingDoubleMuonLowMass5/Run2023C-PromptReco-v4/MINIAOD
/ParkingDoubleMuonLowMass5/Run2023D-PromptReco-v1/MINIAOD
/ParkingDoubleMuonLowMass5/Run2023D-PromptReco-v2/MINIAOD
/ParkingDoubleMuonLowMass6/Run2023B-PromptReco-v1/MINIAOD
/ParkingDoubleMuonLowMass6/Run2023C-PromptReco-v1/MINIAOD
/ParkingDoubleMuonLowMass6/Run2023C-PromptReco-v2/MINIAOD
/ParkingDoubleMuonLowMass6/Run2023C-PromptReco-v3/MINIAOD
/ParkingDoubleMuonLowMass6/Run2023C-PromptReco-v4/MINIAOD
/ParkingDoubleMuonLowMass6/Run2023D-PromptReco-v1/MINIAOD
/ParkingDoubleMuonLowMass6/Run2023D-PromptReco-v2/MINIAOD
/ParkingDoubleMuonLowMass7/Run2023B-PromptReco-v1/MINIAOD
/ParkingDoubleMuonLowMass7/Run2023C-PromptReco-v1/MINIAOD
/ParkingDoubleMuonLowMass7/Run2023C-PromptReco-v2/MINIAOD
/ParkingDoubleMuonLowMass7/Run2023C-PromptReco-v3/MINIAOD
/ParkingDoubleMuonLowMass7/Run2023C-PromptReco-v4/MINIAOD
/ParkingDoubleMuonLowMass7/Run2023D-PromptReco-v1/MINIAOD
/ParkingDoubleMuonLowMass7/Run2023D-PromptReco-v2/MINIAOD
```


### Contributors
* Wang Chi (Eric100911), undergraduate at Zhili College, Tsinghua University.
* Cheng Xing, undergraduate at Zhili College, Tsinghua University.
* Shi Zhenpeng, undergraduate at Dept. of Physics, Tsinghua University.

Supervised under Prof. Hu Zhen at Dept. of Physics, Tsinghua University.

## Overview

### Event Selection Procedure

1. Match $\mu^+\mu^-$ pairs using vertex fitting. (Track geometry only.)

2. Create $J/\psi$ and $\Upsilon$ candidates using $\mu^+\mu^-$ pairs. (Dynamics required. Mass window, pT selection and other restrictions required.)

3. Matching $J/\psi$ and $\Upsilon$ candidates from one single vertex . (Track geometry only. May check $c\tau$ distribution.)

### Efficiency

### Systematics

## Code Framework

### Event Selection Procedure

1. `void LoadMC()` Load MC results if `doMC == true`.
2. Some other code for initialization
3. Import trigger results and save for possible trigger matching.
4. Harvest muons from tracks. Loop over muon pairs.
5. For muon pair candidataes, apply a crude selection with "opposite-charge criterion" and mass window cut. ( For $J/\psi$, consider mass range $[1.0, 4.0]$. For $\Upsilon$, consider mass range $[8.0, 12.0]$. )
6. Apply kinematic fitting for each pair to vertices. 
7. Store valid muon pairs by category. ($J/\psi$ or $\Upsilon$)
8. Use non-overlapping muon pairs to form $J/\psi$ and $\Upsilon$ candidates. (Careful not to directly store iterators! Use pointers instead.)
9. Store the candidates and corresponding muon pairs.

### Efficiency

### Systematics

