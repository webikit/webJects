import random

def generate_payload(length):
    """Generate a random payload (byte array) of a given length."""
    return [random.randint(0, 255) for _ in range(length)]

def generate_payloads(num_payloads):
    """Generate a list of random payloads with lengths up to 16 bytes."""
    return [generate_payload(random.randint(1, 16)) for _ in range(num_payloads)]

def format_payload(payload):
    """Format a single payload as a C-style byte array."""
    formatted_payload = '{' + ', '.join(f'0x{byte:02x}' for byte in payload) + '}'
    # Pad the payload to always be 16 bytes for consistency in array definition
    if len(payload) < 1024:
        padding = ', '.join('0x00' for _ in range(16 - len(payload)))
        formatted_payload = formatted_payload[:-1] + ', ' + padding + '}'
    return formatted_payload

def main():
    # Define the number of payloads
    num_payloads = int(input("Enter the number of payloads to generate: "))

    # Generate the payloads
    payloads = generate_payloads(num_payloads)

    # Print the C-style payload array definition
    print("\nunsigned char payloads[][16] = {")
    for payload in payloads:
        print(f"    {format_payload(payload)},")
    print("};")

if __name__ == "__main__":
    main()
