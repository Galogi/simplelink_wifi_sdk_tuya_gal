This directory contains demonstration certificates used for testing WPA Enterprise and secured socket functionalities within the SDK.
It also includes generate_global_from_pem.py, a Python script that converts these PEM files into C header files (.h) containing a hex byte array global variable, suitable for direct use in C applications (e.g., the network terminal app).
Usage Example:
To generate the header file for clientPrivKey.pem using the variable name client_private_key:

python3 generate_global_from_pem.py client_private_key clientPrivKey.pem