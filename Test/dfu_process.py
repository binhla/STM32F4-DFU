import serial
import sys, getopt

def process_hex_file(hex_file, com_port):
    uart = serial.Serial()
    print("Open file " + hex_file)
    file = open(hex_file)
    if not check_file(file):
        print("Error: file not in hex format")
        return False
    print("continue process")
    file.close()
    uart.baudrate = 115200
    #uart.baudrate = 57600
    #uart.baudrate = 460800
    #uart.baudrate = 1152000
    #uart.stopbits = serial.STOPBITS_TWO
    uart.port = com_port
    #uart.xonxoff  = True
    #uart.parity  = serial.PARITY_EVEN
    uart.open()
    boothook_count = 0
    while True:
        uart_send_boothook(uart)
        if uart_wait_ack(uart, 1):
            break;
        boothook_count += 1
        if boothook_count > 5:
            print("Error: boothook failed")
            uart.close()
            return False
    file = open(hex_file)
    line_num = 0
        
    for line in file:
        line_num += 1
        print("line " + str(line_num))
        if not process_line(uart, line[:-1]):
            print("Error: process line failed")
            uart.close()
            file.close()
            return False
    file.close()
    print("process hex file done")
    reboot_count = 0
    while True:
        uart_send_reboot(uart)
        if uart_wait_ack(uart, 1):
            break
        reboot_count += 1
        if reboot_count > 5:
            break
    uart.close()
    return True

def process_line(uart, line):
    try_count = 0
    while True:
        line_length = len(line)
        #print("line length " + str(line_length))
        #print(line)
        uart_send_hex_line(uart, line)
        if uart_wait_ack(uart, 1):
            return True
        try_count += 1
        if try_count >= 10:
            break;
    return False

def is_hex_line(line):
    line_length = len(line)
    if line[0] != ':':
        return False
    for i in range(1, line_length):
        if is_hex_char(line[i]) == False:
            return False
    if line_length % 2 == 0:
        return False
    return True

def is_hex_char(xC):
    if xC in ['0', '1', '2', '3', '4', '5', '6', '7', '8', '9']:
        return True
    if xC in ['a', 'b', 'c', 'd', 'e', 'f']:
        return True
    if xC in ['A', 'B', 'C', 'D', 'E', 'F']:
        return True
    return False

def check_file(file):
    for line in file:
        if not is_hex_line(line[:-1]):
            return False
    return True

def uart_send_boothook(uart):
    print("Send boot hook")
    boothook_msg = ":00000011EF#"
    uart.write(boothook_msg.encode('utf-8'))
    return

def uart_wait_ack(uart, time_out):
    #return True
    ack_msg = "00000012EE#"
    nack_msg = ":00000013ED#"
    uart.timeout = time_out
    rx_bytes = uart.read_until(expected=b'#');
    try:
	    rx_str = str(rx_bytes, 'utf-8')
	    ack_idx = rx_str.find(ack_msg)
	    if (ack_idx != -1):
	        return True
	    nack_idx = rx_str.find(nack_msg)
	    if (nack_idx == -1):
	        print("Error: timeout")
	    else:
	        print("Error: receive NACK")
    except UnicodeError:
	    print("Error: Unicode error")
    return False

def uart_send_hex_line(uart, line):
    line = line + '#'
    uart.write(line.encode('utf-8'))
    return

def uart_send_reboot(uart):
    print("Send reboot")
    reboot_msg = ":00000015EB#"
    uart.write(reboot_msg.encode('utf-8'))
    return

def main(argv):
    try:
        opts, args = getopt.getopt(argv, "p:f:", ["port = ", "file = "])
    except:
        print(sys.argv[0] + " -p <COM_PORT> -f <Hex File>")
        sys.exit()
    com_port = ""
    hex_file = ""
    for opt, arg in opts:
        if opt == '-p':
            com_port = arg
        elif opt == '-f':
            hex_file = arg
    if not com_port or not hex_file:
        print(sys.argv[0] + " -p <COM_PORT> -f <Hex File>")
        sys.exit()
    process_hex_file(hex_file, com_port)
		

if __name__ == "__main__":
    main(sys.argv[1:])
	#py dfu_process.py -p com7 -f blink_iwdg.hex
	#py dfu_process.py -p com7 -f blink.hex
    #py dfu_process.py -p com7 -f VOR02_FIRMWARE_DEV.hex

