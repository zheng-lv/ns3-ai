# Copyright (c) 2023 Huazhong University of Science and Technology
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License version 2 as
# published by the Free Software Foundation;
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#这个脚本的目的是告诉 pip 安装此包所需的信息，以及指定依赖项。当用户运行 pip install . 或 pip install ns3ai_gym_env 时，将会根据这个脚本配置进行安装。
# Author: Muyuan Shen <muyuan_shen@hust.edu.cn>

#导入 setuptools 模块，用于构建和分发 Python 包。
from setuptools import setup

setup(
    name="ns3ai_gym_env",
    version="0.0.1",
    install_requires=["numpy", "gymnasium", "protobuf==3.20.3"],
)
