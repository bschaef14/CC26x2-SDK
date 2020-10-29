import setuptools

setuptools.setup(
    name="tilogger-itm-transport",
    version="0.0.1",
    # author="Core SDK team",
    packages=setuptools.find_packages(),
    include_package_data=True,
    entry_points={
        "tilogger.transport": [
            "itm = tilogger_itm_transport.itm_transport:ITM_Transport",
        ],
        "console_scripts": [
            "tilogger_itm_viewer = tilogger_itm_transport.itm_transport:itm_parser_main"
        ],
    },
    install_requires=[
        "pyserial"
    ],
)
