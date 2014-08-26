import os
import re


list_dirs = os.walk('.')
print list_dirs
for root, dirs, files in list_dirs:
    for f in files:
        if re.search('tar$', f):
            filepath = os.path.join(root, f).replace('\\', '/')
            basepath = '.' + filepath.strip('Download.tar')
            print basepath
            os.system('tar -xvf ' + filepath + ' -C ' + basepath)
