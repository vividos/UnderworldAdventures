#! /usr/bin/env python3
""" Testcases for zziplib build system """

__copyright__ = "(C) Guido Draheim, all rights reserved"""
__version__ = "0.13.72"

import subprocess
import os.path
import time
import datetime
import unittest
import shutil
import inspect
import types
import logging
import re
from fnmatch import fnmatchcase as fnmatch
from glob import glob
import json
import sys

if sys.version[0] == '3':
    basestring = str
    xrange = range

logg = logging.getLogger("TESTING")
_python = "/usr/bin/python"

SAVETO = "localhost:5000/zziplib"
IMAGES = "localhost:5000/zziplib/image"
CENTOS8 = "centos:8.3.2011"
CENTOS7 = "centos:7.9.2009"
UBUNTU1 = "ubuntu:18.04"
UBUNTU2 = "ubuntu:16.04"
UBUNTU3 = "ubuntu:20.04"
OPENSUSE5 = "opensuse/leap:15.2"
SOFTWARE = "../Software"

DOCKER_SOCKET = "/var/run/docker.sock"
DOCKER = "docker"
FORCE = False
NOCACHE = False

MAINDIR = os.path.dirname(sys.argv[0]) or "."
MIRROR = os.path.join(MAINDIR, "docker_mirror.py")

def decodes(text):
    if text is None: return None
    if isinstance(text, bytes):
        encoded = sys.getdefaultencoding()
        if encoded in ["ascii"]:
            encoded = "utf-8"
        try: 
            return text.decode(encoded)
        except:
            return text.decode("latin-1")
    return text
def sh____(cmd, shell=True):
    if isinstance(cmd, basestring):
        logg.info(": %s", cmd)
    else:    
        logg.info(": %s", " ".join(["'%s'" % item for item in cmd]))
    return subprocess.check_call(cmd, shell=shell)
def sx____(cmd, shell=True):
    if isinstance(cmd, basestring):
        logg.info(": %s", cmd)
    else:    
        logg.info(": %s", " ".join(["'%s'" % item for item in cmd]))
    return subprocess.call(cmd, shell=shell)
def output(cmd, shell=True):
    if isinstance(cmd, basestring):
        logg.info(": %s", cmd)
    else:    
        logg.info(": %s", " ".join(["'%s'" % item for item in cmd]))
    run = subprocess.Popen(cmd, shell=shell, stdout=subprocess.PIPE)
    out, err = run.communicate()
    return decodes(out)
def output2(cmd, shell=True):
    if isinstance(cmd, basestring):
        logg.info(": %s", cmd)
    else:    
        logg.info(": %s", " ".join(["'%s'" % item for item in cmd]))
    run = subprocess.Popen(cmd, shell=shell, stdout=subprocess.PIPE)
    out, err = run.communicate()
    return decodes(out), run.returncode
def output3(cmd, shell=True):
    if isinstance(cmd, basestring):
        logg.info(": %s", cmd)
    else:    
        logg.info(": %s", " ".join(["'%s'" % item for item in cmd]))
    run = subprocess.Popen(cmd, shell=shell, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    out, err = run.communicate()
    return decodes(out), decodes(err), run.returncode
def background(cmd, shell=True):
    BackgroundProcess = collections.namedtuple("BackgroundProcess", ["pid", "run", "log" ])
    log = open(os.devnull, "wb")
    run = subprocess.Popen(cmd, shell=shell, stdout=log, stderr=log)
    pid = run.pid
    logg.info("PID %s = %s", pid, cmd)
    return BackgroundProcess(pid, run, log)



def _lines(lines):
    if isinstance(lines, basestring):
        lines = lines.split("\n")
        if len(lines) and lines[-1] == "":
            lines = lines[:-1]
    return lines
def lines(text):
    lines = []
    for line in _lines(text):
        lines.append(line.rstrip())
    return lines
def grep(pattern, lines):
    for line in _lines(lines):
       if re.search(pattern, line.rstrip()):
           yield line.rstrip()
def greps(lines, pattern):
    return list(grep(pattern, lines))

def download(base_url, filename, into):
    if not os.path.isdir(into):
        os.makedirs(into)
    if not os.path.exists(os.path.join(into, filename)):
        sh____("cd {into} && wget {base_url}/{filename}".format(**locals()))
def text_file(filename, content):
    filedir = os.path.dirname(filename)
    if not os.path.isdir(filedir):
        os.makedirs(filedir)
    f = open(filename, "w")
    if content.startswith("\n"):
        x = re.match("(?s)\n( *)", content)
        indent = x.group(1)
        for line in content[1:].split("\n"):
            if line.startswith(indent):
                line = line[len(indent):]
            f.write(line+"\n")
    else:
        f.write(content)
    f.close()
def shell_file(filename, content):
    text_file(filename, content)
    os.chmod(filename, 0o770)
def copy_file(filename, target):
    targetdir = os.path.dirname(target)
    if not os.path.isdir(targetdir):
        os.makedirs(targetdir)
    shutil.copyfile(filename, target)
def copy_tool(filename, target):
    copy_file(filename, target)
    os.chmod(target, 0o750)

def get_caller_name():
    frame = inspect.currentframe().f_back.f_back
    return frame.f_code.co_name
def get_caller_caller_name():
    frame = inspect.currentframe().f_back.f_back.f_back
    return frame.f_code.co_name
def os_path(root, path):
    if not root:
        return path
    if not path:
        return path
    while path.startswith(os.path.sep):
       path = path[1:]
    return os.path.join(root, path)
def docname(path):
    return os.path.splitext(os.path.basename(path))[0]

def link_software(software, parts):
    software = software or "Software"
    shelf = SOFTWARE
    for part in parts.split(","):
        item = os.path.join(shelf, part)
        if os.path.isdir(item):
            for dirpath, dirnames, filenames in os.walk(item):
                basepath = dirpath.replace(shelf+"/", "")
                for filename in filenames:
                    intofile = os.path.join(software, basepath, filename)
                    fromfile = os.path.join(dirpath, filename)
                    intodir = os.path.dirname(intofile)
                    if not os.path.isdir(intodir):
                        os.makedirs(intodir)
                    if not os.path.isfile(intofile):
                        os.link(fromfile, intofile)
def unlink_software(software, parts):
    software = software or "Software"
    shelf = SOFTWARE
    for part in parts.split(","):
        item = os.path.join(shelf, part)
        if os.path.isdir(item):
            for dirpath, dirnames, filenames in os.walk(item):
                basepath = dirpath.replace(shelf+"/", "")
                for filename in filenames:
                    intofile = os.path.join(software, basepath, filename)
                    if os.path.isfile(intofile):
                        os.unlink(intofile)

class ZZiplibBuildTest(unittest.TestCase):
    def caller_testname(self):
        name = get_caller_caller_name()
        x1 = name.find("_")
        if x1 < 0: return name
        x2 = name.find("_", x1+1)
        if x2 < 0: return name
        return name[:x2]
    def testname(self, suffix = None):
        name = self.caller_testname()
        if suffix:
            return name + "_" + suffix
        return name
    def testdir(self, testname = None):
        testname = testname or self.caller_testname()
        newdir = "tmp/tmp."+testname
        if os.path.isdir(newdir):
            shutil.rmtree(newdir)
        os.makedirs(newdir)
        return newdir
    def rm_old(self, testname = None):
        testname = testname or self.caller_testname()
        for subdir in ("docs/man3", "docs/html"):
            if os.path.isdir(subdir):
                logg.info("rm -rf %s", subdir)
                shutil.rmtree(subdir)
        for filename in ("tmp.local.tgz", "docs/htmpages.tar", "docs/manpages.tar", "docs/zziplib.xml", "docs/zzipmmapped.xml", "docs/zzipfseeko.xml"):
            if os.path.isfile(filename):
                logg.info("rm %s", filename)
                os.remove(filename)
    def rm_testdir(self, testname = None):
        testname = testname or self.caller_testname()
        newdir = "tmp/tmp."+testname
        if os.path.isdir(newdir):
            shutil.rmtree(newdir)
        return newdir
    def makedirs(self, path):
        if not os.path.isdir(path):
            os.makedirs(path)
    def user(self):
        import getpass
        getpass.getuser()
    def ip_container(self, name):
        values = output("docker inspect "+name)
        values = json.loads(values)
        if not values or "NetworkSettings" not in values[0]:
            logg.critical(" docker inspect %s => %s ", name, values)
        return values[0]["NetworkSettings"]["IPAddress"]    
    def local_image(self, image):
        """ attach local centos-repo / opensuse-repo to docker-start enviroment.
            Effectivly when it is required to 'docker start centos:x.y' then do
            'docker start centos-repo:x.y' before and extend the original to 
            'docker start --add-host mirror...:centos-repo centos:x.y'. """
        if os.environ.get("NONLOCAL",""):
            return image
        add_hosts = self.start_mirror(image)
        if add_hosts:
            return "{add_hosts} {image}".format(**locals())
        return image
    def local_addhosts(self, dockerfile, extras = None):
        image = ""
        for line in open(dockerfile):
            m = re.match('[Ff][Rr][Oo][Mm] *"([^"]*)"', line)
            if m: 
                image = m.group(1)
                break
            m = re.match("[Ff][Rr][Oo][Mm] *(\w[^ ]*)", line)
            if m: 
                image = m.group(1).strip()
                break
        logg.debug("--\n-- '%s' FROM '%s'", dockerfile, image)
        if image:
            return self.start_mirror(image, extras)
        return ""
    def start_mirror(self, image, extras = None):
        extras = extras or ""
        docker = DOCKER
        mirror = MIRROR
        cmd = "{mirror} start {image} --add-hosts {extras}"
        out = output(cmd.format(**locals()))
        return decodes(out).strip()
    def nocache(self):
        if FORCE or NOCACHE:
            return " --no-cache"
        return ""
    #
    # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
    #
    def test_101(self):
        logg.info("\n  UBUNTU1 = '%s'", UBUNTU1)
        self.start_mirror(UBUNTU1)
    def test_102(self):
        logg.info("\n  UBUNTU2 = '%s'", UBUNTU2)
        self.start_mirror(UBUNTU2)
    def test_103(self):
        logg.info("\n  UBUNTU3 = '%s'", UBUNTU3)
        self.start_mirror(UBUNTU3, "--universe")
    def test_105(self):
        logg.info("\n  OPENSUSE5 = '%s'", OPENSUSE5)
        self.start_mirror(OPENSUSE5)
    def test_107(self):
        logg.info("\n  CENTOS7 = '%s'", CENTOS7)
        self.start_mirror(CENTOS7, "--epel")
    def test_108(self):
        logg.info("\n  CENTOS8 = '%s'", CENTOS8)
        self.start_mirror(CENTOS8)
    def test_207_centos7_automake_dockerfile(self):
        if not os.path.exists(DOCKER_SOCKET): self.skipTest("docker-based test")
        self.rm_old()
        self.rm_testdir()
        testname=self.testname()
        testdir = self.testdir()
        dockerfile="testbuilds/centos7-am-build.dockerfile"
        addhosts = self.local_addhosts(dockerfile, "--epel")
        savename = docname(dockerfile)
        saveto = SAVETO
        images = IMAGES
        build = "build --build-arg=no_check=true" + self.nocache()
        cmd = "docker {build} . -f {dockerfile} {addhosts} --tag {images}:{testname}"
        sh____(cmd.format(**locals()))
        cmd = "docker rm --force {testname}"
        sx____(cmd.format(**locals()))
        cmd = "docker run -d --name {testname} {images}:{testname} sleep 60"
        sh____(cmd.format(**locals()))
        #:# container = self.ip_container(testname)
        cmd = "docker exec {testname} ls -l /usr/local/bin"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname} find /usr/local/include -type f"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname} bash -c 'ls -l /usr/local/lib64/libzz*'"
        sh____(cmd.format(**locals()))
        #
        cmd = "docker exec {testname} bash -c 'test ! -d /usr/local/include/SDL_rwops_zzip'"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname} rpm -q --whatprovides /usr/lib64/pkgconfig/zlib.pc"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname} pkg-config --libs zlib"
        zlib = output(cmd.format(**locals()))
        self.assertEqual(zlib.strip(), "-lz")
        #
        cmd = "docker rm --force {testname}"
        sx____(cmd.format(**locals()))
        cmd = "docker rmi {saveto}/{savename}:latest"
        sx____(cmd.format(**locals()))
        cmd = "docker tag {images}:{testname} {saveto}/{savename}:latest"
        sh____(cmd.format(**locals()))
        cmd = "docker rmi {images}:{testname}"
        sx____(cmd.format(**locals()))
        self.rm_testdir()
    def test_208_centos8_automake_dockerfile(self):
        if not os.path.exists(DOCKER_SOCKET): self.skipTest("docker-based test")
        self.rm_old()
        self.rm_testdir()
        testname=self.testname()
        testdir = self.testdir()
        dockerfile="testbuilds/centos8-am-build.dockerfile"
        addhosts = self.local_addhosts(dockerfile)
        savename = docname(dockerfile)
        saveto = SAVETO
        images = IMAGES
        build = "build --build-arg=no_check=true" + self.nocache()
        cmd = "docker {build} . -f {dockerfile} {addhosts} --tag {images}:{testname}"
        sh____(cmd.format(**locals()))
        cmd = "docker rm --force {testname}"
        sx____(cmd.format(**locals()))
        cmd = "docker run -d --name {testname} {images}:{testname} sleep 60"
        sh____(cmd.format(**locals()))
        #:# container = self.ip_container(testname)
        cmd = "docker exec {testname} ls -l /usr/local/bin"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname} find /usr/local/include -type f"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname} bash -c 'ls -l /usr/local/lib64/libzz*'"
        sh____(cmd.format(**locals()))
        #
        cmd = "docker exec {testname} bash -c 'test ! -d /usr/local/include/SDL_rwops_zzip'"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname} rpm -q --whatprovides /usr/lib64/pkgconfig/zlib.pc"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname} pkg-config --libs zlib"
        zlib = output(cmd.format(**locals()))
        self.assertEqual(zlib.strip(), "-lz")
        #
        cmd = "docker rm --force {testname}"
        sx____(cmd.format(**locals()))
        cmd = "docker rmi {saveto}/{savename}:latest"
        sx____(cmd.format(**locals()))
        cmd = "docker tag {images}:{testname} {saveto}/{savename}:latest"
        sh____(cmd.format(**locals()))
        cmd = "docker rmi {images}:{testname}"
        sx____(cmd.format(**locals()))
        self.rm_testdir()
    def test_217_centos7_cmake_build_dockerfile(self):
        if not os.path.exists(DOCKER_SOCKET): self.skipTest("docker-based test")
        self.rm_old()
        self.rm_testdir()
        testname=self.testname()
        testdir = self.testdir()
        dockerfile="testbuilds/centos7-cm-build.dockerfile"
        addhosts = self.local_addhosts(dockerfile)
        savename = docname(dockerfile)
        saveto = SAVETO
        images = IMAGES
        build = "build --build-arg=no_check=true" + self.nocache()
        cmd = "docker {build} . -f {dockerfile} {addhosts} --tag {images}:{testname}"
        sh____(cmd.format(**locals()))
        cmd = "docker rm --force {testname}"
        sx____(cmd.format(**locals()))
        cmd = "docker run -d --name {testname} {images}:{testname} sleep 60"
        sh____(cmd.format(**locals()))
        #:# container = self.ip_container(testname)
        cmd = "docker exec {testname} ls -l /usr/local/bin"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname} find /usr/local/include -type f"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname} bash -c 'ls -l /usr/local/lib64/libzz*'"
        sh____(cmd.format(**locals()))
        #
        cmd = "docker exec {testname} bash -c 'test ! -d /usr/local/include/SDL_rwops_zzip'"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname} rpm -q --whatprovides /usr/lib64/pkgconfig/zlib.pc"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname} pkg-config --libs zlib"
        zlib = output(cmd.format(**locals()))
        self.assertEqual(zlib.strip(), "-lz")
        #
        cmd = "docker rm --force {testname}"
        sx____(cmd.format(**locals()))
        cmd = "docker rmi {saveto}/{savename}:latest"
        sx____(cmd.format(**locals()))
        cmd = "docker tag {images}:{testname} {saveto}/{savename}:latest"
        sh____(cmd.format(**locals()))
        cmd = "docker rmi {images}:{testname}"
        sx____(cmd.format(**locals()))
        self.rm_testdir()
    def test_218_centos8_cmake_build_dockerfile(self):
        if not os.path.exists(DOCKER_SOCKET): self.skipTest("docker-based test")
        self.rm_old()
        self.rm_testdir()
        testname=self.testname()
        testdir = self.testdir()
        dockerfile="testbuilds/centos8-cm-build.dockerfile"
        addhosts = self.local_addhosts(dockerfile)
        savename = docname(dockerfile)
        saveto = SAVETO
        images = IMAGES
        build = "build --build-arg=no_check=true" + self.nocache()
        cmd = "docker {build} . -f {dockerfile} {addhosts} --tag {images}:{testname}"
        sh____(cmd.format(**locals()))
        cmd = "docker rm --force {testname}"
        sx____(cmd.format(**locals()))
        cmd = "docker run -d --name {testname} {images}:{testname} sleep 60"
        sh____(cmd.format(**locals()))
        #:# container = self.ip_container(testname)
        cmd = "docker exec {testname} ls -l /usr/local/bin"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname} find /usr/local/include -type f"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname} bash -c 'ls -l /usr/local/lib64/libzz*'"
        sh____(cmd.format(**locals()))
        #
        cmd = "docker exec {testname} bash -c 'test ! -d /usr/local/include/SDL_rwops_zzip'"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname} rpm -q --whatprovides /usr/lib64/pkgconfig/zlib.pc"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname} pkg-config --libs zlib"
        zlib = output(cmd.format(**locals()))
        self.assertEqual(zlib.strip(), "-lz")
        #
        cmd = "docker rm --force {testname}"
        sx____(cmd.format(**locals()))
        cmd = "docker rmi {saveto}/{savename}:latest"
        sx____(cmd.format(**locals()))
        cmd = "docker tag {images}:{testname} {saveto}/{savename}:latest"
        sh____(cmd.format(**locals()))
        cmd = "docker rmi {images}:{testname}"
        sx____(cmd.format(**locals()))
        self.rm_testdir()
    def test_221_ubuntu16_cmake_build_dockerfile(self):
        if not os.path.exists(DOCKER_SOCKET): self.skipTest("docker-based test")
        self.rm_old()
        self.rm_testdir()
        testname=self.testname()
        testdir = self.testdir()
        dockerfile="testbuilds/ubuntu16-cm-build.dockerfile"
        addhosts = self.local_addhosts(dockerfile)
        savename = docname(dockerfile)
        saveto = SAVETO
        images = IMAGES
        build = "build --build-arg=no_check=true" + self.nocache()
        cmd = "docker {build} . -f {dockerfile} {addhosts} --tag {images}:{testname}"
        sh____(cmd.format(**locals()))
        cmd = "docker rm --force {testname}"
        sx____(cmd.format(**locals()))
        cmd = "docker run -d --name {testname} {images}:{testname} sleep 600"
        sh____(cmd.format(**locals()))
        #:# container = self.ip_container(testname)
        cmd = "docker exec {testname} ls -l /usr/local/bin"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname} find /usr/local/include -type f"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname} bash -c 'ls -l /usr/local/lib/libzz*'"
        sh____(cmd.format(**locals()))
        #
        cmd = "docker exec {testname} bash -c 'test ! -d /usr/local/include/SDL_rwops_zzip'"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname} dpkg -S /usr/lib/x86_64-linux-gnu/pkgconfig/zlib.pc"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname} pkg-config --libs zlib"
        zlib = output(cmd.format(**locals()))
        self.assertEqual(zlib.strip(), "-lz")
        #
        cmd = "docker rm --force {testname}"
        sx____(cmd.format(**locals()))
        cmd = "docker rmi {saveto}/{savename}:latest"
        sx____(cmd.format(**locals()))
        cmd = "docker tag {images}:{testname} {saveto}/{savename}:latest"
        sh____(cmd.format(**locals()))
        cmd = "docker rmi {images}:{testname}"
        sx____(cmd.format(**locals()))
        self.rm_testdir()
    def test_222_ubuntu18_cmake_build_dockerfile(self):
        if not os.path.exists(DOCKER_SOCKET): self.skipTest("docker-based test")
        self.rm_old()
        self.rm_testdir()
        testname=self.testname()
        testdir = self.testdir()
        dockerfile="testbuilds/ubuntu18-cm-build.dockerfile"
        addhosts = self.local_addhosts(dockerfile)
        savename = docname(dockerfile)
        saveto = SAVETO
        images = IMAGES
        build = "build --build-arg=no_check=true" + self.nocache()
        cmd = "docker {build} . -f {dockerfile} {addhosts} --tag {images}:{testname}"
        sh____(cmd.format(**locals()))
        cmd = "docker rm --force {testname}"
        sx____(cmd.format(**locals()))
        cmd = "docker run -d --name {testname} {images}:{testname} sleep 600"
        sh____(cmd.format(**locals()))
        #:# container = self.ip_container(testname)
        cmd = "docker exec {testname} ls -l /usr/local/bin"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname} find /usr/local/include -type f"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname} bash -c 'ls -l /usr/local/lib/libzz*'"
        sh____(cmd.format(**locals()))
        #
        cmd = "docker exec {testname} bash -c 'test ! -d /usr/local/include/SDL_rwops_zzip'"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname} dpkg -S /usr/lib/x86_64-linux-gnu/pkgconfig/zlib.pc"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname} pkg-config --libs zlib"
        zlib = output(cmd.format(**locals()))
        self.assertEqual(zlib.strip(), "-lz")
        #
        cmd = "docker rm --force {testname}"
        sx____(cmd.format(**locals()))
        cmd = "docker rmi {saveto}/{savename}:latest"
        sx____(cmd.format(**locals()))
        cmd = "docker tag {images}:{testname} {saveto}/{savename}:latest"
        sh____(cmd.format(**locals()))
        cmd = "docker rmi {images}:{testname}"
        sx____(cmd.format(**locals()))
        self.rm_testdir()
    def test_224_ubuntu16_32bit_dockerfile(self):
        if not os.path.exists(DOCKER_SOCKET): self.skipTest("docker-based test")
        self.rm_old()
        self.rm_testdir()
        testname=self.testname()
        testdir = self.testdir()
        dockerfile="testbuilds/ubuntu16-32bit.dockerfile"
        addhosts = self.local_addhosts(dockerfile)
        savename = docname(dockerfile)
        saveto = SAVETO
        images = IMAGES
        build = "build --build-arg=no_check=true" + self.nocache()
        cmd = "docker {build} . -f {dockerfile} {addhosts} --tag {images}:{testname}"
        sh____(cmd.format(**locals()))
        cmd = "docker rm --force {testname}"
        sx____(cmd.format(**locals()))
        cmd = "docker run -d --name {testname} {images}:{testname} sleep 600"
        sh____(cmd.format(**locals()))
        #:# container = self.ip_container(testname)
        cmd = "docker exec {testname} ls -l /usr/local/bin"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname} find /usr/local/include -type f"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname} bash -c 'ls -l /usr/local/lib/libzz*'"
        sh____(cmd.format(**locals()))
        #
        cmd = "docker exec {testname} bash -c 'test ! -d /usr/local/include/SDL_rwops_zzip'"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname} dpkg -S /usr/lib/i386-linux-gnu/pkgconfig/zlib.pc"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname} pkg-config --libs zlib"
        zlib = output(cmd.format(**locals()))
        self.assertEqual(zlib.strip(), "-lz")
        #
        cmd = "docker rm --force {testname}"
        sx____(cmd.format(**locals()))
        cmd = "docker rmi {saveto}/{savename}:latest"
        sx____(cmd.format(**locals()))
        cmd = "docker tag {images}:{testname} {saveto}/{savename}:latest"
        sh____(cmd.format(**locals()))
        cmd = "docker rmi {images}:{testname}"
        sx____(cmd.format(**locals()))
        self.rm_testdir()
    def test_235_opensuse15_cmake_build_dockerfile(self):
        if not os.path.exists(DOCKER_SOCKET): self.skipTest("docker-based test")
        self.rm_old()
        self.rm_testdir()
        testname=self.testname()
        testdir = self.testdir()
        dockerfile="testbuilds/opensuse15-cm-build.dockerfile"
        addhosts = self.local_addhosts(dockerfile)
        savename = docname(dockerfile)
        saveto = SAVETO
        images = IMAGES
        build = "build --build-arg=no_check=true" + self.nocache()
        cmd = "docker {build} . -f {dockerfile} {addhosts} --tag {images}:{testname}"
        sh____(cmd.format(**locals()))
        cmd = "docker rm --force {testname}"
        sx____(cmd.format(**locals()))
        cmd = "docker run -d --name {testname} {images}:{testname} sleep 60"
        sh____(cmd.format(**locals()))
        #:# container = self.ip_container(testname)
        cmd = "docker exec {testname} ls -l /usr/local/bin"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname} find /usr/local/include -type f"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname} bash -c 'ls -l /usr/local/lib64/libzz*'"
        sh____(cmd.format(**locals()))
        #
        cmd = "docker exec {testname} bash -c 'test ! -d /usr/local/include/SDL_rwops_zzip'"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname} rpm -q --whatprovides /usr/lib64/pkgconfig/zlib.pc"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname} pkg-config --libs zlib"
        zlib = output(cmd.format(**locals()))
        self.assertEqual(zlib.strip(), "-lz")
        #
        cmd = "docker rm --force {testname}"
        sx____(cmd.format(**locals()))
        cmd = "docker rmi {saveto}/{savename}:latest"
        sx____(cmd.format(**locals()))
        cmd = "docker tag {images}:{testname} {saveto}/{savename}:latest"
        sh____(cmd.format(**locals()))
        cmd = "docker rmi {images}:{testname}"
        sx____(cmd.format(**locals()))
        self.rm_testdir()
    @unittest.expectedFailure
    def test_250_windows_static_x64_dockerfile(self):
        logg.warning("     windows-static-x64 compiles fine but segfaults on linking an .exe")
        if not os.path.exists(DOCKER_SOCKET): self.skipTest("docker-based test")
        self.rm_old()
        self.rm_testdir()
        testname=self.testname()
        testdir = self.testdir()
        dockerfile="testbuilds/windows-static-x64.dockerfile"
        addhosts = self.local_addhosts(dockerfile)
        savename = docname(dockerfile)
        saveto = SAVETO
        images = IMAGES
        build = "build --build-arg=no_check=true" + self.nocache()
        cmd = "docker {build} . -f {dockerfile} {addhosts} --tag {images}:{testname}"
        sh____(cmd.format(**locals()))
        cmd = "docker rm --force {testname}"
        sx____(cmd.format(**locals()))
        cmd = "docker run -d --name {testname} {images}:{testname} sleep 600"
        sh____(cmd.format(**locals()))
        #:# container = self.ip_container(testname)
        #
        cmd = "docker exec {testname} ls -l /usr/local/bin"
        sh____(cmd.format(**locals()))
        #
        cmd = "docker rm --force {testname}"
        sx____(cmd.format(**locals()))
        cmd = "docker rmi {saveto}/{savename}:latest"
        sx____(cmd.format(**locals()))
        cmd = "docker tag {images}:{testname} {saveto}/{savename}:latest"
        sh____(cmd.format(**locals()))
        cmd = "docker rmi {images}:{testname}"
        sx____(cmd.format(**locals()))
        self.rm_testdir()
    @unittest.expectedFailure
    def test_260_windows_shared_x64_dockerfile(self):
        logg.warning("     windows-shared-x64 compiles fine but segfaults on linking an .exe")
        if not os.path.exists(DOCKER_SOCKET): self.skipTest("docker-based test")
        self.rm_old()
        self.rm_testdir()
        testname=self.testname()
        testdir = self.testdir()
        dockerfile="testbuilds/windows-shared-x64.dockerfile"
        addhosts = self.local_addhosts(dockerfile)
        savename = docname(dockerfile)
        saveto = SAVETO
        images = IMAGES
        build = "build --build-arg=no_check=true" + self.nocache()
        cmd = "docker {build} . -f {dockerfile} {addhosts} --tag {images}:{testname}"
        sh____(cmd.format(**locals()))
        cmd = "docker rm --force {testname}"
        sx____(cmd.format(**locals()))
        cmd = "docker run -d --name {testname} {images}:{testname} sleep 600"
        sh____(cmd.format(**locals()))
        #:# container = self.ip_container(testname)
        #
        cmd = "docker exec {testname} ls -l /usr/local/bin"
        sh____(cmd.format(**locals()))
        #
        cmd = "docker rm --force {testname}"
        sx____(cmd.format(**locals()))
        cmd = "docker rmi {saveto}/{savename}:latest"
        sx____(cmd.format(**locals()))
        cmd = "docker tag {images}:{testname} {saveto}/{savename}:latest"
        sh____(cmd.format(**locals()))
        cmd = "docker rmi {images}:{testname}"
        sx____(cmd.format(**locals()))
        self.rm_testdir()
    def test_307_centos7_automake_sdl2_dockerfile(self):
        if not os.path.exists(DOCKER_SOCKET): self.skipTest("docker-based test")
        self.rm_old()
        self.rm_testdir()
        testname=self.testname()
        testdir = self.testdir()
        dockerfile="testbuilds/centos7-am-sdl2.dockerfile"
        addhosts = self.local_addhosts(dockerfile)
        savename = docname(dockerfile)
        saveto = SAVETO
        images = IMAGES
        build = "build --build-arg=no_check=true" + self.nocache()
        cmd = "docker {build} . -f {dockerfile} {addhosts} --tag {images}:{testname}"
        sh____(cmd.format(**locals()))
        cmd = "docker rm --force {testname}"
        sx____(cmd.format(**locals()))
        cmd = "docker run -d --name {testname} {images}:{testname} sleep 60"
        sh____(cmd.format(**locals()))
        #:# container = self.ip_container(testname)
        cmd = "docker exec {testname} ls -l /usr/local/bin"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname} find /usr/local/include -type f"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname} bash -c 'ls -l /usr/local/lib64/libzz*'"
        sh____(cmd.format(**locals()))
        #
        cmd = "docker exec {testname} bash -c 'test -d /usr/local/include/SDL_rwops_zzip'"
        sh____(cmd.format(**locals()))
        #
        cmd = "docker rm --force {testname}"
        sx____(cmd.format(**locals()))
        cmd = "docker rmi {saveto}/{savename}:latest"
        sx____(cmd.format(**locals()))
        cmd = "docker tag {images}:{testname} {saveto}/{savename}:latest"
        sh____(cmd.format(**locals()))
        cmd = "docker rmi {images}:{testname}"
        sx____(cmd.format(**locals()))
        self.rm_testdir()
    def test_308_centos8_automake_sdl2_dockerfile(self):
        if not os.path.exists(DOCKER_SOCKET): self.skipTest("docker-based test")
        self.rm_old()
        self.rm_testdir()
        testname=self.testname()
        testdir = self.testdir()
        dockerfile="testbuilds/centos8-am-sdl2.dockerfile"
        addhosts = self.local_addhosts(dockerfile)
        savename = docname(dockerfile)
        saveto = SAVETO
        images = IMAGES
        build = "build --build-arg=no_check=true" + self.nocache()
        cmd = "docker {build} . -f {dockerfile} {addhosts} --tag {images}:{testname}"
        sh____(cmd.format(**locals()))
        cmd = "docker rm --force {testname}"
        sx____(cmd.format(**locals()))
        cmd = "docker run -d --name {testname} {images}:{testname} sleep 60"
        sh____(cmd.format(**locals()))
        #:# container = self.ip_container(testname)
        cmd = "docker exec {testname} ls -l /usr/local/bin"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname} find /usr/local/include -type f"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname} bash -c 'ls -l /usr/local/lib64/libzz*'"
        sh____(cmd.format(**locals()))
        #
        cmd = "docker exec {testname} bash -c 'test -d /usr/local/include/SDL_rwops_zzip'"
        sh____(cmd.format(**locals()))
        #
        cmd = "docker rm --force {testname}"
        sx____(cmd.format(**locals()))
        cmd = "docker rmi {saveto}/{savename}:latest"
        sx____(cmd.format(**locals()))
        cmd = "docker tag {images}:{testname} {saveto}/{savename}:latest"
        sh____(cmd.format(**locals()))
        cmd = "docker rmi {images}:{testname}"
        sx____(cmd.format(**locals()))
        self.rm_testdir()
    def test_317_centos7_cmake_sdl2_dockerfile(self):
        if not os.path.exists(DOCKER_SOCKET): self.skipTest("docker-based test")
        self.rm_old()
        self.rm_testdir()
        testname=self.testname()
        testdir = self.testdir()
        dockerfile="testbuilds/centos7-cm-sdl2.dockerfile"
        addhosts = self.local_addhosts(dockerfile)
        savename = docname(dockerfile)
        saveto = SAVETO
        images = IMAGES
        build = "build --build-arg=no_check=true" + self.nocache()
        cmd = "docker {build} . -f {dockerfile} {addhosts} --tag {images}:{testname}"
        sh____(cmd.format(**locals()))
        cmd = "docker rm --force {testname}"
        sx____(cmd.format(**locals()))
        cmd = "docker run -d --name {testname} {images}:{testname} sleep 60"
        sh____(cmd.format(**locals()))
        #:# container = self.ip_container(testname)
        cmd = "docker exec {testname} ls -l /usr/local/bin"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname} find /usr/local/include -type f"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname} bash -c 'ls -l /usr/local/lib64/libzz*'"
        sh____(cmd.format(**locals()))
        #
        cmd = "docker exec {testname} bash -c 'test -d /usr/local/include/SDL_rwops_zzip'"
        sh____(cmd.format(**locals()))
        #
        cmd = "docker rm --force {testname}"
        sx____(cmd.format(**locals()))
        cmd = "docker rmi {saveto}/{savename}:latest"
        sx____(cmd.format(**locals()))
        cmd = "docker tag {images}:{testname} {saveto}/{savename}:latest"
        sh____(cmd.format(**locals()))
        cmd = "docker rmi {images}:{testname}"
        sx____(cmd.format(**locals()))
        self.rm_testdir()
    def test_318_centos8_cmake_sdl2_dockerfile(self):
        if not os.path.exists(DOCKER_SOCKET): self.skipTest("docker-based test")
        self.rm_old()
        self.rm_testdir()
        testname=self.testname()
        testdir = self.testdir()
        dockerfile="testbuilds/centos8-cm-sdl2.dockerfile"
        addhosts = self.local_addhosts(dockerfile)
        savename = docname(dockerfile)
        saveto = SAVETO
        images = IMAGES
        build = "build --build-arg=no_check=true" + self.nocache()
        cmd = "docker {build} . -f {dockerfile} {addhosts} --tag {images}:{testname}"
        sh____(cmd.format(**locals()))
        cmd = "docker rm --force {testname}"
        sx____(cmd.format(**locals()))
        cmd = "docker run -d --name {testname} {images}:{testname} sleep 60"
        sh____(cmd.format(**locals()))
        #:# container = self.ip_container(testname)
        cmd = "docker exec {testname} ls -l /usr/local/bin"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname} find /usr/local/include -type f"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname} bash -c 'ls -l /usr/local/lib64/libzz*'"
        sh____(cmd.format(**locals()))
        #
        cmd = "docker exec {testname} bash -c 'test -d /usr/local/include/SDL_rwops_zzip'"
        sh____(cmd.format(**locals()))
        #
        cmd = "docker rm --force {testname}"
        sx____(cmd.format(**locals()))
        cmd = "docker rmi {saveto}/{savename}:latest"
        sx____(cmd.format(**locals()))
        cmd = "docker tag {images}:{testname} {saveto}/{savename}:latest"
        sh____(cmd.format(**locals()))
        cmd = "docker rmi {images}:{testname}"
        sx____(cmd.format(**locals()))
        self.rm_testdir()
    def test_322_ubuntu18_cmake_sdl2_dockerfile(self):
        if not os.path.exists(DOCKER_SOCKET): self.skipTest("docker-based test")
        self.rm_old()
        self.rm_testdir()
        testname=self.testname()
        testdir = self.testdir()
        dockerfile="testbuilds/ubuntu16-cm-sdl2.dockerfile"
        addhosts = self.local_addhosts(dockerfile, "--universe")
        savename = docname(dockerfile)
        saveto = SAVETO
        images = IMAGES
        build = "build --build-arg=no_check=true" + self.nocache()
        cmd = "docker {build} . -f {dockerfile} {addhosts} --tag {images}:{testname}"
        sh____(cmd.format(**locals()))
        cmd = "docker rm --force {testname}"
        sx____(cmd.format(**locals()))
        cmd = "docker run -d --name {testname} {images}:{testname} sleep 600"
        sh____(cmd.format(**locals()))
        #:# container = self.ip_container(testname)
        cmd = "docker exec {testname} ls -l /usr/local/bin"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname} find /usr/local/include -type f"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname} bash -c 'ls -l /usr/local/lib/libzz*'"
        sh____(cmd.format(**locals()))
        #
        cmd = "docker exec {testname} bash -c 'test -d /usr/local/include/SDL_rwops_zzip'"
        sh____(cmd.format(**locals()))
        #
        cmd = "docker rm --force {testname}"
        sx____(cmd.format(**locals()))
        cmd = "docker rmi {saveto}/{savename}:latest"
        sx____(cmd.format(**locals()))
        cmd = "docker tag {images}:{testname} {saveto}/{savename}:latest"
        sh____(cmd.format(**locals()))
        cmd = "docker rmi {images}:{testname}"
        sx____(cmd.format(**locals()))
        self.rm_testdir()
    def test_335_opensuse15_cmake_sdl2_dockerfile(self):
        if not os.path.exists(DOCKER_SOCKET): self.skipTest("docker-based test")
        self.rm_old()
        self.rm_testdir()
        testname=self.testname()
        testdir = self.testdir()
        dockerfile="testbuilds/opensuse15-cm-sdl2.dockerfile"
        addhosts = self.local_addhosts(dockerfile)
        savename = docname(dockerfile)
        saveto = SAVETO
        images = IMAGES
        build = "build --build-arg=no_check=true" + self.nocache()
        cmd = "docker {build} . -f {dockerfile} {addhosts} --tag {images}:{testname}"
        sh____(cmd.format(**locals()))
        cmd = "docker rm --force {testname}"
        sx____(cmd.format(**locals()))
        cmd = "docker run -d --name {testname} {images}:{testname} sleep 60"
        sh____(cmd.format(**locals()))
        #:# container = self.ip_container(testname)
        cmd = "docker exec {testname} ls -l /usr/local/bin"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname} find /usr/local/include -type f"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname} bash -c 'ls -l /usr/local/lib64/libzz*'"
        sh____(cmd.format(**locals()))
        #
        cmd = "docker exec {testname} bash -c 'test -d /usr/local/include/SDL_rwops_zzip'"
        sh____(cmd.format(**locals()))
        #
        cmd = "docker rm --force {testname}"
        sx____(cmd.format(**locals()))
        cmd = "docker rmi {saveto}/{savename}:latest"
        sx____(cmd.format(**locals()))
        cmd = "docker tag {images}:{testname} {saveto}/{savename}:latest"
        sh____(cmd.format(**locals()))
        cmd = "docker rmi {images}:{testname}"
        sx____(cmd.format(**locals()))
        self.rm_testdir()
    def test_417_centos7_cmake_sdl2_destdir_dockerfile(self):
        if not os.path.exists(DOCKER_SOCKET): self.skipTest("docker-based test")
        self.rm_old()
        self.rm_testdir()
        testname=self.testname()
        testdir = self.testdir()
        dockerfile="testbuilds/centos7-cm-destdir-sdl2.dockerfile"
        addhosts = self.local_addhosts(dockerfile)
        savename = docname(dockerfile)
        saveto = SAVETO
        images = IMAGES
        build = "build --build-arg=no_check=true" + self.nocache()
        cmd = "docker {build} . -f {dockerfile} {addhosts} --tag {images}:{testname}"
        sh____(cmd.format(**locals()))
        cmd = "docker rm --force {testname}"
        sx____(cmd.format(**locals()))
        cmd = "docker run -d --name {testname} {images}:{testname} sleep 60"
        sh____(cmd.format(**locals()))
        #:# container = self.ip_container(testname)
        cmd = "docker exec {testname} ls -l /new/usr/local/bin"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname} find /new/usr/local/include -type f"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname} bash -c 'ls -l /new/usr/local/lib64/libzz*'"
        sh____(cmd.format(**locals()))
        #
        cmd = "docker exec {testname} bash -c 'test -d /new/usr/local/include/SDL_rwops_zzip'"
        sh____(cmd.format(**locals()))
        #
        cmd = "docker rm --force {testname}"
        sx____(cmd.format(**locals()))
        cmd = "docker rmi {saveto}/{savename}:latest"
        sx____(cmd.format(**locals()))
        cmd = "docker tag {images}:{testname} {saveto}/{savename}:latest"
        sh____(cmd.format(**locals()))
        cmd = "docker rmi {images}:{testname}"
        sx____(cmd.format(**locals()))
        self.rm_testdir()
    def test_418_centos8_cmake_sdl2_destdir_dockerfile(self):
        if not os.path.exists(DOCKER_SOCKET): self.skipTest("docker-based test")
        self.rm_old()
        self.rm_testdir()
        testname=self.testname()
        testdir = self.testdir()
        dockerfile="testbuilds/centos8-cm-destdir-sdl2.dockerfile"
        addhosts = self.local_addhosts(dockerfile)
        savename = docname(dockerfile)
        saveto = SAVETO
        images = IMAGES
        build = "build --build-arg=no_check=true" + self.nocache()
        cmd = "docker {build} . -f {dockerfile} {addhosts} --tag {images}:{testname}"
        sh____(cmd.format(**locals()))
        cmd = "docker rm --force {testname}"
        sx____(cmd.format(**locals()))
        cmd = "docker run -d --name {testname} {images}:{testname} sleep 60"
        sh____(cmd.format(**locals()))
        #:# container = self.ip_container(testname)
        cmd = "docker exec {testname} ls -l /new/usr/local/bin"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname} find /new/usr/local/include -type f"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname} bash -c 'ls -l /new/usr/local/lib64/libzz*'"
        sh____(cmd.format(**locals()))
        #
        cmd = "docker exec {testname} bash -c 'test -d /new/usr/local/include/SDL_rwops_zzip'"
        sh____(cmd.format(**locals()))
        #
        cmd = "docker rm --force {testname}"
        sx____(cmd.format(**locals()))
        cmd = "docker rmi {saveto}/{savename}:latest"
        sx____(cmd.format(**locals()))
        cmd = "docker tag {images}:{testname} {saveto}/{savename}:latest"
        sh____(cmd.format(**locals()))
        cmd = "docker rmi {images}:{testname}"
        sx____(cmd.format(**locals()))
        self.rm_testdir()
    def test_423_ubuntu20_azure_dockerfile(self):
        if not os.path.exists(DOCKER_SOCKET): self.skipTest("docker-based test")
        self.rm_old()
        self.rm_testdir()
        testname=self.testname()
        testdir = self.testdir()
        dockerfile="testbuilds/ubuntu20-azure.dockerfile"
        addhosts = self.local_addhosts(dockerfile, "--universe")
        savename = docname(dockerfile)
        saveto = SAVETO
        images = IMAGES
        build = "build --build-arg=no_install=true" + self.nocache()
        cmd = "docker {build} . -f {dockerfile} {addhosts} --tag {images}:{testname}"
        sh____(cmd.format(**locals()))
        cmd = "docker rm --force {testname}"
        sx____(cmd.format(**locals()))
        cmd = "docker run -d --name {testname} {images}:{testname} sleep 600"
        sh____(cmd.format(**locals()))
        #:# container = self.ip_container(testname)
        #
        cmd = "docker exec {testname} find src -name *.xml"
        sh____(cmd.format(**locals()))
        #
        cmd = "docker rm --force {testname}"
        sx____(cmd.format(**locals()))
        cmd = "docker rmi {saveto}/{savename}:latest"
        sx____(cmd.format(**locals()))
        cmd = "docker tag {images}:{testname} {saveto}/{savename}:latest"
        sh____(cmd.format(**locals()))
        cmd = "docker rmi {images}:{testname}"
        sx____(cmd.format(**locals()))
        self.rm_testdir()
    @unittest.expectedFailure
    def test_424_ubuntu20_azure_dockerfile(self):
        if not os.path.exists(DOCKER_SOCKET): self.skipTest("docker-based test")
        self.rm_old()
        self.rm_testdir()
        dockerfile="testbuilds/ubuntu20-azure.dockerfile" # reusing test_423
        savename = docname(dockerfile)
        saveto = SAVETO
        images = IMAGES
        testname=self.testname()
        cmd = "docker rm --force {testname}"
        sx____(cmd.format(**locals()))
        cmd = "docker run -d --name {testname} {saveto}/{savename} sleep 60"
        sh____(cmd.format(**locals()))
        #:# container = self.ip_container(testname)
        #
        logg.error("\n\n\t Ubuntu ships 'unzip' that can not handle these CVE zip files .. creating core dumps\n\n")
        cmd = "docker exec {testname} bash -c 'cd src/build/test && python3 ../../test/zziptests.py test_65430'"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname} bash -c 'cd src/build/test && python3 ../../test/zziptests.py test_65440'"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname} bash -c 'cd src/build/test && python3 ../../test/zziptests.py test_65480'"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname} bash -c 'cd src/build/test && python3 ../../test/zziptests.py test_65485'"
        sh____(cmd.format(**locals()))
        #
        cmd = "docker rm --force {testname}"
        sx____(cmd.format(**locals()))
        self.rm_testdir()
    def test_435_opensuse15_ninja_sdl2_dockerfile(self):
        if not os.path.exists(DOCKER_SOCKET): self.skipTest("docker-based test")
        self.rm_old()
        self.rm_testdir()
        testname=self.testname()
        testdir = self.testdir()
        dockerfile="testbuilds/opensuse15-nj-sdl2.dockerfile"
        addhosts = self.local_addhosts(dockerfile)
        savename = docname(dockerfile)
        saveto = SAVETO
        images = IMAGES
        build = "build --build-arg=no_check=true" + self.nocache()
        cmd = "docker {build} . -f {dockerfile} {addhosts} --tag {images}:{testname}"
        sh____(cmd.format(**locals()))
        cmd = "docker rm --force {testname}"
        sx____(cmd.format(**locals()))
        cmd = "docker run -d --name {testname} {images}:{testname} sleep 60"
        sh____(cmd.format(**locals()))
        #:# container = self.ip_container(testname)
        cmd = "docker exec {testname} ls -l /usr/local/bin"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname} find /usr/local/include -type f"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname} bash -c 'ls -l /usr/local/lib64/libzz*'"
        sh____(cmd.format(**locals()))
        #
        cmd = "docker exec {testname} bash -c 'test -d /usr/local/include/SDL_rwops_zzip'"
        sh____(cmd.format(**locals()))
        #
        cmd = "docker rm --force {testname}"
        sx____(cmd.format(**locals()))
        cmd = "docker rmi {saveto}/{savename}:latest"
        sx____(cmd.format(**locals()))
        cmd = "docker tag {images}:{testname} {saveto}/{savename}:latest"
        sh____(cmd.format(**locals()))
        cmd = "docker rmi {images}:{testname}"
        sx____(cmd.format(**locals()))
        self.rm_testdir()
    def test_707_centos7_automake_docs_dockerfile(self):
        if not os.path.exists(DOCKER_SOCKET): self.skipTest("docker-based test")
        self.rm_old()
        self.rm_testdir()
        testname=self.testname()
        testdir = self.testdir()
        dockerfile="testbuilds/centos7-am-docs.dockerfile"
        addhosts = self.local_addhosts(dockerfile)
        savename = docname(dockerfile)
        saveto = SAVETO
        images = IMAGES
        build = "build --build-arg=no_build=true" + self.nocache()
        cmd = "docker {build} . -f {dockerfile} {addhosts} --tag {images}:{testname}"
        sh____(cmd.format(**locals()))
        cmd = "docker rm --force {testname}"
        sx____(cmd.format(**locals()))
        cmd = "docker run -d --name {testname} {images}:{testname} sleep 60"
        sh____(cmd.format(**locals()))
        #:# container = self.ip_container(testname)
        cmd = "docker exec {testname} ls -l /usr/local/bin"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname} find /usr/local/include -type f"
        sh____(cmd.format(**locals()))
        #
        cmd = "docker exec {testname} bash -c 'test ! -d /usr/local/include/zzip/types.h'"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname} bash -c 'test -d /usr/local/share/doc/zziplib'"
        sh____(cmd.format(**locals()))    
        cmd = "docker exec {testname} bash -c 'test -f /usr/local/share/man/man3/zzip_opendir.3'"
        sh____(cmd.format(**locals()))
        #
        cmd = "docker rm --force {testname}"
        sx____(cmd.format(**locals()))
        cmd = "docker rmi {saveto}/{savename}:latest"
        sx____(cmd.format(**locals()))
        cmd = "docker tag {images}:{testname} {saveto}/{savename}:latest"
        sh____(cmd.format(**locals()))
        cmd = "docker rmi {images}:{testname}"
        sx____(cmd.format(**locals()))
        self.rm_testdir()
    def test_717_centos7_cmake_docs_dockerfile(self):
        if not os.path.exists(DOCKER_SOCKET): self.skipTest("docker-based test")
        self.rm_old()
        self.rm_testdir()
        testname=self.testname()
        testdir = self.testdir()
        dockerfile="testbuilds/centos7-cm-docs.dockerfile"
        addhosts = self.local_addhosts(dockerfile)
        savename = docname(dockerfile)
        saveto = SAVETO
        images = IMAGES
        build = "build" # "build --build-arg=no_build=true"
        build += self.nocache()
        cmd = "docker {build} . -f {dockerfile} {addhosts} --tag {images}:{testname}"
        sh____(cmd.format(**locals()))
        cmd = "docker rm --force {testname}"
        sx____(cmd.format(**locals()))
        cmd = "docker run -d --name {testname} {images}:{testname} sleep 600"
        sh____(cmd.format(**locals()))
        #:# container = self.ip_container(testname)
        cmd = "docker exec {testname} ls -l /usr/local/bin"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname} find /usr/local/include -type f"
        sh____(cmd.format(**locals()))
        #
        cmd = "docker exec {testname} bash -c 'test ! -d /usr/local/include/zzip/types.h'"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname} bash -c 'test -d /usr/local/share/doc/zziplib'"
        sh____(cmd.format(**locals()))    
        cmd = "docker exec {testname} bash -c 'test -f /usr/local/share/man/man3/zzip_opendir.3'"
        sh____(cmd.format(**locals()))
        #
        cmd = "docker rm --force {testname}"
        sx____(cmd.format(**locals()))
        cmd = "docker rmi {saveto}/{savename}:latest"
        sx____(cmd.format(**locals()))
        cmd = "docker tag {images}:{testname} {saveto}/{savename}:latest"
        sh____(cmd.format(**locals()))
        cmd = "docker rmi {images}:{testname}"
        sx____(cmd.format(**locals()))
        self.rm_testdir()
    def test_917_centos7_am_cm_dockerfile(self):
        if not os.path.exists(DOCKER_SOCKET): self.skipTest("docker-based test")
        self.rm_old()
        self.rm_testdir()
        testname1=self.testname() + "_am"
        testname2=self.testname() + "_cm"
        testdir = self.testdir()
        dockerfile1="testbuilds/centos7-am-build.dockerfile" # make st_207
        dockerfile2="testbuilds/centos7-cm-build.dockerfile" # make st_217
        addhosts = self.local_addhosts(dockerfile1)
        savename1 = docname(dockerfile1)
        savename2 = docname(dockerfile2)
        saveto = SAVETO
        images = IMAGES
        cmd = "docker rm --force {testname1}"
        sx____(cmd.format(**locals()))
        cmd = "docker rm --force {testname2}"
        sx____(cmd.format(**locals()))
        cmd = "docker run -d --name {testname1} {addhosts} {saveto}/{savename1} sleep 600"
        sh____(cmd.format(**locals()))
        cmd = "docker run -d --name {testname2} {addhosts} {saveto}/{savename2} sleep 600"
        #
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname2} bash -c 'cd /usr/local && tar czvf /local.tgz .'"
        sh____(cmd.format(**locals()))
        cmd = "docker cp {testname2}:/local.tgz tmp.local.tgz"
        sh____(cmd.format(**locals()))
        cmd = "docker cp tmp.local.tgz {testname1}:/local.tgz"
        sh____(cmd.format(**locals()))
        cmd = "rm tmp.local.tgz"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname1} mkdir -p /new/local"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname1} bash -c 'cd /new/local && tar xzvf /local.tgz'"
        sh____(cmd.format(**locals()))
        #
        item="{}"
        end="\\;"
        A='"s:zzip-zlib-config:zlib:"'
        B='"s:=/usr/local/:=\\${prefix}/:"'
        C1='"/^exec_prefix=/d"'
        C2='"/^datarootdir=/d"'
        C3='"/^datadir=/d"'
        C4='"/^sysconfdir=/d"'
        C5='"/^bindir=/d"'
        G='"/ generated by configure /d"'
        cmd = "docker exec {testname1} bash -c 'find /usr/local -name *.pc -exec sed -i -e {A} -e {B} -e {C1} -e {C2} -e {C3} -e {C4} -e {C5} -e {G} {item} {end}'"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname1} bash -c 'find /usr/local -name zzip-zlib-config.pc -exec rm -v {item} {end}'"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname1} bash -c 'find /usr/local -name *.la -exec rm -v {item} {end}'"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname1} bash -c 'find /new/local -name *-0.so -exec rm -v {item} {end}'"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname1} diff -uw /usr/local/include/zzip/_config.h /new/local/include/zzip/_config.h"
        sx____(cmd.format(**locals()))
        cmd = "docker exec {testname1} diff -urw --no-dereference /usr/local /new/local --exclude _config.h"
        sx____(cmd.format(**locals()))
        out = output(cmd.format(**locals()))
        if "---" in out or "Only" in out:
            logg.warning("out>>\n%s", out)
        self.assertFalse(greps(out, "---"))
        self.assertFalse(greps(out, "Only"))
        #
        cmd = "docker rm --force {testname1}"
        sx____(cmd.format(**locals()))
        cmd = "docker rm --force {testname2}"
        sx____(cmd.format(**locals()))
        self.rm_testdir()
    def test_918_centos8_am_cm_dockerfile(self):
        if not os.path.exists(DOCKER_SOCKET): self.skipTest("docker-based test")
        self.rm_old()
        self.rm_testdir()
        testname1=self.testname() + "_am"
        testname2=self.testname() + "_cm"
        testdir = self.testdir()
        dockerfile1="testbuilds/centos8-am-build.dockerfile" # make st_208
        dockerfile2="testbuilds/centos8-cm-build.dockerfile" # make st_218
        addhosts = self.local_addhosts(dockerfile1)
        savename1 = docname(dockerfile1)
        savename2 = docname(dockerfile2)
        saveto = SAVETO
        images = IMAGES
        cmd = "docker rm --force {testname1}"
        sx____(cmd.format(**locals()))
        cmd = "docker rm --force {testname2}"
        sx____(cmd.format(**locals()))
        cmd = "docker run -d --name {testname1} {addhosts} {saveto}/{savename1} sleep 600"
        sh____(cmd.format(**locals()))
        cmd = "docker run -d --name {testname2} {addhosts} {saveto}/{savename2} sleep 600"
        #
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname2} bash -c 'cd /usr/local && tar czvf /local.tgz .'"
        sh____(cmd.format(**locals()))
        cmd = "docker cp {testname2}:/local.tgz tmp.local.tgz"
        sh____(cmd.format(**locals()))
        cmd = "docker cp tmp.local.tgz {testname1}:/local.tgz"
        sh____(cmd.format(**locals()))
        cmd = "rm tmp.local.tgz"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname1} mkdir -p /new/local"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname1} bash -c 'cd /new/local && tar xzvf /local.tgz'"
        sh____(cmd.format(**locals()))
        #
        item="{}"
        end="\\;"
        A='"s:zzip-zlib-config:zlib:"'
        B='"s:=/usr/local/:=\\${prefix}/:"'
        C1='"/^exec_prefix=/d"'
        C2='"/^datarootdir=/d"'
        C3='"/^datadir=/d"'
        C4='"/^sysconfdir=/d"'
        C5='"/^bindir=/d"'
        G='"/ generated by configure /d"'
        cmd = "docker exec {testname1} bash -c 'find /usr/local -name *.pc -exec sed -i -e {A} -e {B} -e {C1} -e {C2} -e {C3} -e {C4} -e {C5} -e {G} {item} {end}'"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname1} bash -c 'find /usr/local -name zzip-zlib-config.pc -exec rm -v {item} {end}'"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname1} bash -c 'find /usr/local -name *.la -exec rm -v {item} {end}'"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname1} bash -c 'find /new/local -name *-0.so -exec rm -v {item} {end}'"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname1} diff -uw /usr/local/include/zzip/_config.h /new/local/include/zzip/_config.h"
        sx____(cmd.format(**locals()))
        cmd = "docker exec {testname1} diff -urw --no-dereference /usr/local /new/local --exclude _config.h"
        sx____(cmd.format(**locals()))
        out = output(cmd.format(**locals()))
        if "---" in out or "Only" in out:
            logg.warning("out>>\n%s", out)
        self.assertFalse(greps(out, "---"))
        self.assertFalse(greps(out, "Only"))
        #
        cmd = "docker rm --force {testname1}"
        sx____(cmd.format(**locals()))
        cmd = "docker rm --force {testname2}"
        sx____(cmd.format(**locals()))
        self.rm_testdir()
    def test_937_centos7_am_cm_sdl2_dockerfile(self):
        if not os.path.exists(DOCKER_SOCKET): self.skipTest("docker-based test")
        self.rm_old()
        self.rm_testdir()
        testname1=self.testname() + "_am"
        testname2=self.testname() + "_cm"
        testdir = self.testdir()
        dockerfile1="testbuilds/centos7-am-sdl2.dockerfile" # make st_307
        dockerfile2="testbuilds/centos7-cm-sdl2.dockerfile" # make st_317
        addhosts = self.local_addhosts(dockerfile1)
        savename1 = docname(dockerfile1)
        savename2 = docname(dockerfile2)
        saveto = SAVETO
        images = IMAGES
        cmd = "docker rm --force {testname1}"
        sx____(cmd.format(**locals()))
        cmd = "docker rm --force {testname2}"
        sx____(cmd.format(**locals()))
        cmd = "docker run -d --name {testname1} {addhosts} {saveto}/{savename1} sleep 600"
        sh____(cmd.format(**locals()))
        cmd = "docker run -d --name {testname2} {addhosts} {saveto}/{savename2} sleep 600"
        #
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname2} bash -c 'cd /usr/local && tar czvf /local.tgz .'"
        sh____(cmd.format(**locals()))
        cmd = "docker cp {testname2}:/local.tgz tmp.local.tgz"
        sh____(cmd.format(**locals()))
        cmd = "docker cp tmp.local.tgz {testname1}:/local.tgz"
        sh____(cmd.format(**locals()))
        cmd = "rm tmp.local.tgz"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname1} mkdir -p /new/local"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname1} bash -c 'cd /new/local && tar xzvf /local.tgz'"
        sh____(cmd.format(**locals()))
        #
        item="{}"
        end="\\;"
        A='"s:zzip-zlib-config:zlib:"'
        B='"s:=/usr/local/:=\\${prefix}/:"'
        C1='"/^exec_prefix=/d"'
        C2='"/^datarootdir=/d"'
        C3='"/^datadir=/d"'
        C4='"/^sysconfdir=/d"'
        C5='"/^bindir=/d"'
        G='"/ generated by configure /d"'
        cmd = "docker exec {testname1} bash -c 'find /usr/local -name *.pc -exec sed -i -e {A} -e {B} -e {C1} -e {C2} -e {C3} -e {C4} -e {C5} -e {G} {item} {end}'"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname1} bash -c 'find /usr/local -name zzip-zlib-config.pc -exec rm -v {item} {end}'"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname1} bash -c 'find /usr/local -name *.la -exec rm -v {item} {end}'"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname1} bash -c 'find /new/local -name *-0.so -exec rm -v {item} {end}'"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname1} diff -uw /usr/local/include/zzip/_config.h /new/local/include/zzip/_config.h"
        sx____(cmd.format(**locals()))
        cmd = "docker exec {testname1} diff -urw --no-dereference /usr/local /new/local --exclude _config.h"
        sx____(cmd.format(**locals()))
        out = output(cmd.format(**locals()))
        if "---" in out or "Only" in out:
            logg.warning("out>>\n%s", out)
        self.assertFalse(greps(out, "---"))
        self.assertFalse(greps(out, "Only"))
        #
        cmd = "docker rm --force {testname1}"
        sx____(cmd.format(**locals()))
        cmd = "docker rm --force {testname2}"
        sx____(cmd.format(**locals()))
        self.rm_testdir()
    def test_938_centos8_am_cm_sdl2_dockerfile(self):
        if not os.path.exists(DOCKER_SOCKET): self.skipTest("docker-based test")
        self.rm_old()
        self.rm_testdir()
        testname1=self.testname() + "_am"
        testname2=self.testname() + "_cm"
        testdir = self.testdir()
        dockerfile1="testbuilds/centos8-am-sdl2.dockerfile" # make st_308
        dockerfile2="testbuilds/centos8-cm-sdl2.dockerfile" # make st_318
        addhosts = self.local_addhosts(dockerfile1)
        savename1 = docname(dockerfile1)
        savename2 = docname(dockerfile2)
        saveto = SAVETO
        images = IMAGES
        cmd = "docker rm --force {testname1}"
        sx____(cmd.format(**locals()))
        cmd = "docker rm --force {testname2}"
        sx____(cmd.format(**locals()))
        cmd = "docker run -d --name {testname1} {addhosts} {saveto}/{savename1} sleep 600"
        sh____(cmd.format(**locals()))
        cmd = "docker run -d --name {testname2} {addhosts} {saveto}/{savename2} sleep 600"
        #
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname2} bash -c 'cd /usr/local && tar czvf /local.tgz .'"
        sh____(cmd.format(**locals()))
        cmd = "docker cp {testname2}:/local.tgz tmp.local.tgz"
        sh____(cmd.format(**locals()))
        cmd = "docker cp tmp.local.tgz {testname1}:/local.tgz"
        sh____(cmd.format(**locals()))
        cmd = "rm tmp.local.tgz"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname1} mkdir -p /new/local"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname1} bash -c 'cd /new/local && tar xzvf /local.tgz'"
        sh____(cmd.format(**locals()))
        #
        item="{}"
        end="\\;"
        A='"s:zzip-zlib-config:zlib:"'
        B='"s:=/usr/local/:=\\${prefix}/:"'
        C1='"/^exec_prefix=/d"'
        C2='"/^datarootdir=/d"'
        C3='"/^datadir=/d"'
        C4='"/^sysconfdir=/d"'
        C5='"/^bindir=/d"'
        G='"/ generated by configure /d"'
        cmd = "docker exec {testname1} bash -c 'find /usr/local -name *.pc -exec sed -i -e {A} -e {B} -e {C1} -e {C2} -e {C3} -e {C4} -e {C5} -e {G} {item} {end}'"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname1} bash -c 'find /usr/local -name zzip-zlib-config.pc -exec rm -v {item} {end}'"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname1} bash -c 'find /usr/local -name *.la -exec rm -v {item} {end}'"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname1} bash -c 'find /new/local -name *-0.so -exec rm -v {item} {end}'"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname1} diff -uw /usr/local/include/zzip/_config.h /new/local/include/zzip/_config.h"
        sx____(cmd.format(**locals()))
        cmd = "docker exec {testname1} diff -urw --no-dereference /usr/local /new/local --exclude _config.h"
        sx____(cmd.format(**locals()))
        out = output(cmd.format(**locals()))
        if "---" in out or "Only" in out:
            logg.warning("out>>\n%s", out)
        self.assertFalse(greps(out, "---"))
        self.assertFalse(greps(out, "Only"))
        #
        cmd = "docker rm --force {testname1}"
        sx____(cmd.format(**locals()))
        cmd = "docker rm --force {testname2}"
        sx____(cmd.format(**locals()))
        self.rm_testdir()
    def test_947_centos7_cm_sdl2_or_destdir_dockerfile(self):
        if not os.path.exists(DOCKER_SOCKET): self.skipTest("docker-based test")
        self.rm_old()
        self.rm_testdir()
        testname1=self.testname() + "_usr"
        testname2=self.testname() + "_new"
        testdir = self.testdir()
        dockerfile1="testbuilds/centos7-cm-sdl2.dockerfile"          # make st_317
        dockerfile2="testbuilds/centos7-cm-destdir-sdl2.dockerfile"  # make st_417
        addhosts = self.local_addhosts(dockerfile1)
        savename1 = docname(dockerfile1)
        savename2 = docname(dockerfile2)
        saveto = SAVETO
        images = IMAGES
        cmd = "docker rm --force {testname1}"
        sx____(cmd.format(**locals()))
        cmd = "docker rm --force {testname2}"
        sx____(cmd.format(**locals()))
        cmd = "docker run -d --name {testname1} {addhosts} {saveto}/{savename1} sleep 600"
        sh____(cmd.format(**locals()))
        cmd = "docker run -d --name {testname2} {addhosts} {saveto}/{savename2} sleep 600"
        #
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname2} bash -c 'cd /new/usr/local && tar czvf /local.tgz .'"
        sh____(cmd.format(**locals()))
        cmd = "docker cp {testname2}:/local.tgz tmp.local.tgz"
        sh____(cmd.format(**locals()))
        cmd = "docker cp tmp.local.tgz {testname1}:/local.tgz"
        sh____(cmd.format(**locals()))
        cmd = "rm tmp.local.tgz"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname1} mkdir -p /new/local"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname1} bash -c 'cd /new/local && tar xzvf /local.tgz'"
        sh____(cmd.format(**locals()))
        #
        DIRS="etc lib libexec sbin games src share/info share/applications share/man/mann"
        for i in xrange(1,10):
           DIRS+=" share/man/man%i share/man/man%ix" % (i,i)
        cmd = "docker exec {testname1} bash -c 'cd /new/local && (for u in {DIRS}; do mkdir -pv $u; done)'"
        sh____(cmd.format(**locals()))
        item="{}"
        end="\\;"
        cmd = "docker exec {testname1} diff -urw --no-dereference /usr/local /new/local"
        sx____(cmd.format(**locals()))
        out = output(cmd.format(**locals()))
        if "---" in out or "Only" in out:
            logg.warning("out>>\n%s", out)
        self.assertFalse(greps(out, "---"))
        self.assertFalse(greps(out, "Only"))
        #
        cmd = "docker rm --force {testname1}"
        sx____(cmd.format(**locals()))
        cmd = "docker rm --force {testname2}"
        sx____(cmd.format(**locals()))
        self.rm_testdir()
    def test_948_centos8_cm_sdl2_or_destdir_dockerfile(self):
        if not os.path.exists(DOCKER_SOCKET): self.skipTest("docker-based test")
        self.rm_old()
        self.rm_testdir()
        testname1=self.testname() + "_usr"
        testname2=self.testname() + "_new"
        testdir = self.testdir()
        dockerfile1="testbuilds/centos8-cm-sdl2.dockerfile"          # make st_318
        dockerfile2="testbuilds/centos8-cm-destdir-sdl2.dockerfile"  # make st_418
        addhosts = self.local_addhosts(dockerfile1)
        savename1 = docname(dockerfile1)
        savename2 = docname(dockerfile2)
        saveto = SAVETO
        images = IMAGES
        cmd = "docker rm --force {testname1}"
        sx____(cmd.format(**locals()))
        cmd = "docker rm --force {testname2}"
        sx____(cmd.format(**locals()))
        cmd = "docker run -d --name {testname1} {addhosts} {saveto}/{savename1} sleep 600"
        sh____(cmd.format(**locals()))
        cmd = "docker run -d --name {testname2} {addhosts} {saveto}/{savename2} sleep 600"
        #
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname2} bash -c 'cd /new/usr/local && tar czvf /local.tgz .'"
        sh____(cmd.format(**locals()))
        cmd = "docker cp {testname2}:/local.tgz tmp.local.tgz"
        sh____(cmd.format(**locals()))
        cmd = "docker cp tmp.local.tgz {testname1}:/local.tgz"
        sh____(cmd.format(**locals()))
        cmd = "rm tmp.local.tgz"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname1} mkdir -p /new/local"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname1} bash -c 'cd /new/local && tar xzvf /local.tgz'"
        sh____(cmd.format(**locals()))
        #
        DIRS="etc lib libexec sbin games src share/info share/applications share/man/mann"
        for i in xrange(1,10):
           DIRS+=" share/man/man%i share/man/man%ix" % (i,i)
        cmd = "docker exec {testname1} bash -c 'cd /new/local && (for u in {DIRS}; do mkdir -pv $u; done)'"
        sh____(cmd.format(**locals()))
        item="{}"
        end="\\;"
        cmd = "docker exec {testname1} diff -urw --no-dereference /usr/local /new/local"
        sx____(cmd.format(**locals()))
        out = output(cmd.format(**locals()))
        if "---" in out or "Only" in out:
            logg.warning("out>>\n%s", out)
        self.assertFalse(greps(out, "---"))
        self.assertFalse(greps(out, "Only"))
        #
        cmd = "docker rm --force {testname1}"
        sx____(cmd.format(**locals()))
        cmd = "docker rm --force {testname2}"
        sx____(cmd.format(**locals()))
        self.rm_testdir()
    def test_965_opensuse_cm_or_nj_sdl2_dockerfile(self):
        if not os.path.exists(DOCKER_SOCKET): self.skipTest("docker-based test")
        self.rm_old()
        self.rm_testdir()
        testname1=self.testname() + "_cm"
        testname2=self.testname() + "_nj"
        testdir = self.testdir()
        dockerfile1="testbuilds/opensuse15-cm-sdl2.dockerfile"  # make st_335
        dockerfile2="testbuilds/opensuse15-nj-sdl2.dockerfile"  # make st_435
        addhosts = self.local_addhosts(dockerfile1)
        savename1 = docname(dockerfile1)
        savename2 = docname(dockerfile2)
        saveto = SAVETO
        images = IMAGES
        cmd = "docker rm --force {testname1}"
        sx____(cmd.format(**locals()))
        cmd = "docker rm --force {testname2}"
        sx____(cmd.format(**locals()))
        cmd = "docker run -d --name {testname1} {addhosts} {saveto}/{savename1} sleep 600"
        sh____(cmd.format(**locals()))
        cmd = "docker run -d --name {testname2} {addhosts} {saveto}/{savename2} sleep 600"
        #
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname2} bash -c 'cd /usr/local && tar czvf /local.tgz .'"
        sh____(cmd.format(**locals()))
        cmd = "docker cp {testname2}:/local.tgz tmp.local.tgz"
        sh____(cmd.format(**locals()))
        cmd = "docker cp tmp.local.tgz {testname1}:/local.tgz"
        sh____(cmd.format(**locals()))
        cmd = "rm tmp.local.tgz"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname1} mkdir -p /new/local"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname1} bash -c 'cd /new/local && tar xzvf /local.tgz'"
        sh____(cmd.format(**locals()))
        #
        item="{}"
        end="\\;"
        cmd = "docker exec {testname1} diff -urw --no-dereference /usr/local /new/local"
        sx____(cmd.format(**locals()))
        out = output(cmd.format(**locals()))
        if "---" in out or "Only" in out:
            logg.warning("out>>\n%s", out)
        self.assertFalse(greps(out, "---"))
        self.assertFalse(greps(out, "Only"))
        #
        cmd = "docker rm --force {testname1}"
        sx____(cmd.format(**locals()))
        cmd = "docker rm --force {testname2}"
        sx____(cmd.format(**locals()))
        self.rm_testdir()
    def test_977_centos7_am_cm_docs_dockerfile(self):
        if not os.path.exists(DOCKER_SOCKET): self.skipTest("docker-based test")
        self.rm_old()
        self.rm_testdir()
        testname1=self.testname() + "_am"
        testname2=self.testname() + "_cm"
        testdir = self.testdir()
        dockerfile1="testbuilds/centos7-am-docs.dockerfile"
        dockerfile2="testbuilds/centos7-cm-docs.dockerfile"
        addhosts = self.local_addhosts(dockerfile1)
        savename1 = docname(dockerfile1)
        savename2 = docname(dockerfile2)
        saveto = SAVETO
        images = IMAGES
        cmd = "docker rm --force {testname1}"
        sx____(cmd.format(**locals()))
        cmd = "docker rm --force {testname2}"
        sx____(cmd.format(**locals()))
        cmd = "docker run -d --name {testname1} {addhosts} {saveto}/{savename1} sleep 600"
        sh____(cmd.format(**locals()))
        cmd = "docker run -d --name {testname2} {addhosts} {saveto}/{savename2} sleep 600"
        #
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname2} bash -c 'cd /usr/local && tar czvf /local.tgz .'"
        sh____(cmd.format(**locals()))
        cmd = "docker cp {testname2}:/local.tgz tmp.local.tgz"
        sh____(cmd.format(**locals()))
        cmd = "docker cp tmp.local.tgz {testname1}:/local.tgz"
        sh____(cmd.format(**locals()))
        cmd = "rm tmp.local.tgz"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname1} mkdir -p /new/local"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname1} bash -c 'cd /new/local && tar xzvf /local.tgz'"
        sh____(cmd.format(**locals()))
        #
        cmd = "docker exec {testname1} bash -c 'cd /usr/local/share/doc/zziplib && mv man/html .'"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname1} bash -c 'cd /usr/local/share/doc/zziplib && rm -rf man'"
        sh____(cmd.format(**locals()))
        cmd = "docker exec {testname1} bash -c 'cd /usr/local/share/doc/zziplib && mv html man'"
        sh____(cmd.format(**locals()))
        item="{}"
        end="\\;"
        cmd = "docker exec {testname1} diff -urw --no-dereference --brief /usr/local /new/local"
        sx____(cmd.format(**locals()))
        out = output(cmd.format(**locals()))
        if "---" in out or "Only" in out:
            logg.warning("out>>\n%s", out)
        self.assertFalse(greps(out, "---"))
        self.assertFalse(greps(out, "Only"))
        #
        cmd = "docker exec {testname1} diff -urw --no-dereference /usr/local /new/local"
        sx____(cmd.format(**locals()))
        #
        cmd = "docker rm --force {testname1}"
        sx____(cmd.format(**locals()))
        cmd = "docker rm --force {testname2}"
        sx____(cmd.format(**locals()))
        self.rm_testdir()


if __name__ == "__main__":
    from optparse import OptionParser
    _o = OptionParser("%prog [options] test*",
       epilog=__doc__.strip().split("\n")[0])
    _o.add_option("-v","--verbose", action="count", default=0,
       help="increase logging level [%default]")
    _o.add_option("-p","--python", metavar="EXE", default=_python,
       help="use another python execution engine [%default]")
    _o.add_option("-D","--docker", metavar="EXE", default=DOCKER,
       help="use another docker execution engine [%default]")
    _o.add_option("-M","--mirror", metavar="EXE", default=MIRROR,
       help="use another docker_mirror.py script [%default]")
    _o.add_option("-f","--force", action="count", default=0,
       help="force the rebuild steps [%default]")
    _o.add_option("-x","--no-cache", action="count", default=0,
       help="force docker build --no-cache [%default]")
    _o.add_option("-l","--logfile", metavar="FILE", default="",
       help="additionally save the output log to a file [%default]")
    _o.add_option("--xmlresults", metavar="FILE", default=None,
       help="capture results as a junit xml file [%default]")
    opt, args = _o.parse_args()
    logging.basicConfig(level = logging.WARNING - opt.verbose * 5)
    #
    _python = opt.python
    DOCKER = opt.docker
    MIRROR = opt.mirror
    FORCE = opt.force
    NOCACHE = opt.no_cache
    #
    logfile = None
    if opt.logfile:
        if os.path.exists(opt.logfile):
           os.remove(opt.logfile)
        logfile = logging.FileHandler(opt.logfile)
        logfile.setFormatter(logging.Formatter("%(levelname)s:%(relativeCreated)d:%(message)s"))
        logging.getLogger().addHandler(logfile)
        logg.info("log diverted to %s", opt.logfile)
    xmlresults = None
    if opt.xmlresults:
        if os.path.exists(opt.xmlresults):
           os.remove(opt.xmlresults)
        xmlresults = open(opt.xmlresults, "w")
        logg.info("xml results into %s", opt.xmlresults)
    #
    # unittest.main()
    suite = unittest.TestSuite()
    if not args: args = [ "test_*" ]
    for arg in args:
        for classname in sorted(globals()):
            if not classname.endswith("Test"):
                continue
            testclass = globals()[classname]
            for method in sorted(dir(testclass)):
                if "*" not in arg: arg += "*"
                if arg.startswith("_"): arg = arg[1:]
                if fnmatch(method, arg):
                    suite.addTest(testclass(method))
    # select runner
    if not logfile:
        if xmlresults:
            import xmlrunner
            Runner = xmlrunner.XMLTestRunner
            Runner(xmlresults).run(suite)
        else:
            Runner = unittest.TextTestRunner
            Runner(verbosity=opt.verbose).run(suite)
    else:
        Runner = unittest.TextTestRunner
        if xmlresults:
            import xmlrunner
            Runner = xmlrunner.XMLTestRunner
        Runner(logfile.stream, verbosity=opt.verbose).run(suite)
