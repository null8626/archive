from setuptools import setup
f = open("./README.md", "r")
setup(
    name = 'geomepydash',
    packages=['gd'],
    version = '0.3.8',
    license='MIT',
    description = 'Simple python wrapper for Geometry Dash API and some GD Tools (Originally made by GD Colon)',   # Give a short description about your library
    long_description=f.read(),
    long_description_content_type='text/markdown',
    author = 'null8626',
    url = 'https://github.com/null8626/geomepydash',
    download_url = 'https://github.com/null8626/geomepydash/archive/0.3.8.tar.gz',
    keywords = ['API', 'WRAPPER', 'GAMES', 'GD'],
    install_requires=[
        'urllib3',
        'requests'
    ],
    classifiers=[
        'Development Status :: 3 - Alpha',
        'Intended Audience :: Developers',
        'Topic :: Software Development :: Build Tools',
        'License :: OSI Approved :: MIT License',
        'Programming Language :: Python :: 3.5',
        'Programming Language :: Python :: 3.6',
        'Programming Language :: Python :: 3.7',
        'Programming Language :: Python :: 3.8'
    ],
)
