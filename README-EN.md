## General Info

Measuring the cross section of prompt $J/\psi+J/\psi+\Upsilon$ prodution in pp collision on CMS.

Aimed at providing measurement result for studying parton correlation in proton and background estimation for BSM searches.

## Development 

Using CMSSW_13_0_2. Inherited most of the code from AliceQuen/Onia2MuMu. (cr. Qin Junkai at Dept. of Physics, Tsinghua University)

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

## Notes For `git` Usage

### Fundamentals: Pulling, Commiting and Pushing

1. `git pull` Pull the latest changes from the remote repository.
2. `git add .` Add all files to the staging area after making changes.
3. `git commit` Commit the changes. A text editor will pop up for you to write the commit message. Alternatively, you can use `git commit -m "<message>"` to write the message directly.
4. `git push` Push the changes to the default remote repository. (Usually `origin`.)

### Branching: Viewing, Creating, Switching and Merging

1. `git branch` List all branches. The current branch is marked with a `*`.
2. `git branch -a` List all branches, including remote branches.
3. `git branch -c <branch>` Create a new branch named `<branch>`. It will be created from the current branch.
4. `git checkout <branch>` Switch to the branch named `<branch>`. Alternatively, you can use `git switch <branch>`.
5. `git switch -c <branch>` A shortcut: create a new branch named `<branch>` and switch to it.
6. `git branch -d <branch>` Delete the branch named `<branch>`. Use `-D` to force deletion.
7. `git merge <branch>` Merge the branch named `<branch>` into the current branch.

### More Notes On Remote Repositories

1. `git remote` List all remote repositories.
2. `git remote add <name> <url>` Add a new remote repository named `<name>` with URL `<url>`.
3. `git remote remove <name>` Remove the remote repository named `<name>`. This only removes the remote repository from the local repository's configuration. It does not delete the remote repository itself.
4. `git push <name>` Push the changes to the remote repository named `<name>`. This is useful when you have multiple remote repositories. In this project, we use `origin` and `Tsinghua`. The former is on GitHub, while the the latter is a mirror repository on Tsinghua's GitLab. They are kept in sync by the project maintainers.
5. `git push --set-upstream <name> <branch>` Push the changes to the remote repository named `<name>` and set the upstream branch to `<branch>`. This is useful when you are pushing to a new branch on the remote repository.

### Working Together

1. Always do `git pull` before starting off today's work!
2. To do some development from someone else's work, consider using `git switch -c <branch>` to begin your own work on a separate branch.
3. After finishing your work, do `git push --set-upstream <name> <branch>` to push your work to the remote repository `<name>`.
4. To merge your work into the main branch, access the remote repository and create a pull request. The project maintainers will review your work and merge it into the main branch.