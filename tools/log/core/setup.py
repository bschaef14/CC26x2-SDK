import setuptools

setuptools.setup(
    name="tilogger",
    version="0.0.1",
    # author="Core SDK team",
    packages=setuptools.find_packages(),
    include_package_data=True,
    entry_points={
        "console_scripts": [
            "tilogger = tilogger.logger:main"
        ]
    },
    install_requires=[
        "pyserial==3.4",
        "pyelftools==0.25",
        "construct==2.9.45",
        "appdirs==1.4.3",
        "pywin32==228"
    ],
)
