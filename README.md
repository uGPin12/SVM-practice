 # SVM practices
libsvmの練習とB4輪講のプログラム引継ぎ.  
Visual studioやC++について細かいところは詳しくないので，ここはこうした方がよいよなど，気になるところがあれば遠慮なくご指摘ください．

## Requirement
- Visual Studio 2019  
    - Nugetパッケージの管理から以下のものをインストール
        - boostfilesytem vc142  
        - Eigen  
        - itk_static_vc142-x64-release 
    - ソリューション構成，ソリューションプラットフォームはそれぞれ，Release,  x64とする  

## Usage
1. filter.batを実行
2. SVM.batを実行  
パスの設定は相対パスを使用しているので，フォルダ構成に注意(.iniファイル参照)．
