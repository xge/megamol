# How to include closed plugins into your repository
Assuming the `megamol-dev` repository is your `upstream` remote.

```
   git fetch upstream
   git checkout upstream/closed-plugins -- .gitmodules
   git checkout upstream/closed-plugins -- plugins/closed-plugins
   git submodule init plugins/closed-plugins
   git submodule update plugins/closed-plugins
```
 
 By checking out files from other branches, these files will automatically added to the next commit.  
 Please make sure to exclude these files before committing. You can exclude added files using
 
 ```
   git reset <file>
 ```

