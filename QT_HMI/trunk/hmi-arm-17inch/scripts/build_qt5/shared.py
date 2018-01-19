import os, shutil, subprocess
from distutils import dir_util

def run(cmd, args=[], cwd=None, env=None, stdout=None):
    cmdLine = [cmd] + args
    p = subprocess.Popen(cmdLine, stdout=stdout, cwd=cwd, env=env)
    cr = p.communicate()
    if p.returncode != 0:
        raise Exception('"%s" failed with exit code %s' % (' '.join(cmdLine), str(p.returncode)))

    return cr

def create_empty(path):
    if os.path.exists(path):
        shutil.rmtree(path)
    dir_util.mkpath(path)
