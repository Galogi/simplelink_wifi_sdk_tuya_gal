import sys
import os

def generate_c_header(var_name, cert_file_path):
    """
    Reads a PEM certificate file and generates a C header file 
    containing the certificate data as a hex byte array.
    """
    header_filename = f"{var_name}.h"
    
    try:
        # Open the file in binary mode to read raw bytes
        with open(cert_file_path, 'rb') as f:
            cert_data = f.read()
    except IOError as e:
        print(f"Error reading certificate file: {e}")
        sys.exit(1)

    # Format the bytes into a C-style hex array string
    hex_data = [f"0x{byte:02x}" for byte in cert_data]
    
    # Join the hex values, adding a newline every 12 elements for readability (like xxd -i output)
    lines = []
    for i in range(0, len(hex_data), 12):
        lines.append(", ".join(hex_data[i:i+12]))
        
    c_array_content = ",\n  ".join(lines)

    # Generate the full header file content
    header_content = f"""
#ifndef _{var_name.upper()}_H_
#define _{var_name.upper()}_H_

unsigned char {var_name}[] = {{
  {c_array_content}
}};
unsigned int {var_name}_len = {len(cert_data)};

#endif /* _{var_name.upper()}_H_ */
"""

    try:
        with open(header_filename, 'w') as f:
            f.write(header_content)
        print(f"Successfully generated C header file: {header_filename}")
        print(f"Certificate array variable name: {var_name}")
        print(f"Certificate length variable name: {var_name}_len")
    except IOError as e:
        print(f"Error writing header file: {e}")
        sys.exit(1)

if __name__ == "__main__":
    # Check if the correct number of arguments are provided
    if len(sys.argv) != 3:
        print("Usage: python generate_cert_header.py <VAR_NAME> <CERT_FILE_PATH>")
        print("Example: python generate_cert_header.py my_cert /path/to/cert.pem")
        sys.exit(1)
        
    variable_name = sys.argv[1]
    certificate_file = sys.argv[2]
    
    generate_c_header(variable_name, certificate_file)