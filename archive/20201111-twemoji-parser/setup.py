from setuptools import setup
setup(
  name = 'twemoji-parser',
  packages = ['twemoji_parser'],
  version = '0.5',
  license='MIT',
  description = 'A python module made on top of PIL that draws emoji from text to image.',
  long_description = open('README.md', 'r', encoding='utf-8').read(),
  long_description_content_type='text/markdown',
  author = 'null8626',
  url = 'https://github.com/null8626/twemoji-parser',
  download_url = 'https://github.com/null8626/twemoji-parser/archive/0.5.tar.gz',
  keywords = ['Emoji', 'Twemoji', 'PIL', 'Image', 'Parser'],
  install_requires=[
    'pillow',
    'emoji',
    'aiohttp'
  ],
  classifiers=[
    'Development Status :: 4 - Beta',
    'Intended Audience :: Developers',
    'Topic :: Software Development :: Build Tools',
    'License :: OSI Approved :: MIT License',
    'Programming Language :: Python :: 3.7',
    'Programming Language :: Python :: 3.8',
    'Programming Language :: Python :: 3.9'
  ],
  python_requires='>=3.7',
)