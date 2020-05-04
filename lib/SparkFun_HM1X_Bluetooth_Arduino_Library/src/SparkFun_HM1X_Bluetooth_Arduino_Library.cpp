/*
  This is a library written for the HM12/13 Bluetooth 4.0 module (with
  an eye towards expansion for support for HM10/11, HM16/17, HM18/19)
  SparkFun sells these at its website: www.sparkfun.com
  Do you like this library? Help support SparkFun. Buy a board!
  https://www.sparkfun.com/products/14840
  https://www.sparkfun.com/products/14839
  Written by Jim Lindblom @ SparkFun Electronics, July 3, 2018
  
  The HM13 modules are dual mode, Bluetooth 4.0 modules. They support
  both BLE and SPP profiles (SPP via enhanced data-rate [EDR]).
  
  This library handles iniitialization and configuration of the module
  as well as polling for connect/disconnect messages.

  It supports interfacing the module via either hardware or software serial
  or an I2C interface (using the Qwiic Bluetooth 4.0 module). Library is
  designed to support both the Qwiic Bluetooth and the Bluetooth Mate 4.0:
  https://github.com/sparkfunX/Bluetooth_Mate_4.0
  https://github.com/sparkfunX/Qwiic_Bluetooth_HC1x

  Development environment specifics:
  Arduino IDE 1.8.5
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.
  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <SparkFun_HM1X_Bluetooth_Arduino_Library.h>

#define CHECK_HM1X_CONNECTION_ON_BEGIN

const int HM1X_DEFAULT_TIMEOUT = 1000;
const int HM1X_RESPONSE_TIMEOUT = 100;
const int HM1X_POLL_DELAY = 10;

const char HM1X_COMMAND_AT[] = "AT";
const char HM1X_COMMAND_RESET[] = "RESET";
const char HM1X_COMMAND_FACTORY_DEFAULTS[] = "RENEW";
const char HM1X_COMMAND_VERSION[] = "VERR";
const char HM1X_COMMAND_INIT_NOTIFY[] = "INIT";
const char HM1X_COMMAND_NOTIFY_INIT[] = "NOTI";
const char HM1X_COMMAND_NOTIFY_MODE[] = "NOTP";
const char HM1X_COMMAND_EDR_NAME[] = "NAME";
const char HM1X_COMMAND_BLE_NAME[] = "NAMB";
const char HM1X_COMMAND_EDR_ADR[] = "ADDE";
const char HM1X_COMMAND_BLE_ADR[] = "ADDB";
const char HM1X_COMMAND_BLE_ADR_SINGLE[] = "ADDR";
const char HM1X_COMMAND_LAST_EDR[] = "RADE";
const char HM1X_COMMAND_LAST_BLE[] = "RADB";
const char HM1X_COMMAND_LAST_SINGLE[] = "RADD";
const char HM1X_COMMAND_CLEAR_BOND_EDR[] = "BONDE";
const char HM1X_COMMAND_CLEAR_BOND_BLE[] = "BONDB";
const char HM1X_COMMAND_CLEAR_ADR_EDR[] = "CLEAE";
const char HM1X_COMMAND_CLEAR_ADR_BLE[] = "CLEAB";
const char HM1X_COMMAND_CLEAR_ADR_SINGLE[] = "CLEAR";
const char HM1X_COMMAND_EDR_MODE[] = "ROLE";
const char HM1X_COMMAND_BLE_MODE[] = "ROLB";
const char HM1X_COMMAND_HIGH_SPEED_SPP[] = "HIGH";
const char HM1X_COMMAND_DUAL_WORK_MODE[] = "DUAL";
const char HM1X_COMMAND_MODULE_WORK_MODE[] = "MODE";
const char HM1X_COMMAND_A_TO_B_MODE[] = "ATOB";
const char HM1X_COMMAND_AUTHENTICATION_MODE[] = "AUTH";
const char HM1X_COMMAND_EDR_PIN_CODE[] = "PINE";
const char HM1X_COMMAND_BLE_PIN_CODE[] = "PINB";
const char HM1X_COMMAND_PIN_CODE_SINGLE[] = "PASS";
const char HM1X_COMMAND_COD[] = "COFD";
const char HM1X_COMMAND_UPDATE_CON_PARAM[] = "COUP";
const char HM1X_COMMAND_IBEACON_SWITCH[] = "IBEA";
const char HM1X_COMMAND_IBEACON_UUID[] = "IBE";
const char HM1X_COMMAND_IBEACON_MAJOR[] = "MAJO";
const char HM1X_COMMAND_IBEACON_MINOR[] = "MINO";
const char HM1X_COMMAND_IBEACON_POWER[] = "MEAS";
const char HM1X_COMMAND_MTU_SIZE[] = "MTUS";
const char HM1X_COMMAND_ADVERT_TYPE[] = "SCAN";
const char HM1X_COMMAND_SAFE_MODE[] = "SAFE";
const char HM1X_COMMAND_BLE_MAC[] = "ONEM";
const char HM1X_COMMAND_SYSTEM_KEY[] = "PIO0";
const char HM1X_COMMAND_SYSTEM_LED[] = "PIO1";
const char HM1X_COMMAND_PIO_STATUS[] = "PIO";
const char HM1X_COMMAND_BLE_WORK_METHOD[] = "RESP";
const char HM1X_COMMAND_EDR_WORK_TYPE[] = "IMME";
const char HM1X_COMMAND_BLE_WORK_TYPE[] = "IMMB";
const char HM1X_COMMAND_START_EDR_WORK[] = "STARE";
const char HM1X_COMMAND_START_BLE_WORK[] = "STARB";
const char HM1X_COMMAND_STOP_EDR_WORK[] = "STOPE";
const char HM1X_COMMAND_STOP_BLE_WORK[] = "STOPB";
const char HM1X_COMMAND_BAUD[] = "BAUD";
const char HM1X_COMMAND_FLOW_CONTROL[] ="FIOW";
const char HM1X_COMMAND_STOP_BITS[] = "STOP";
const char HM1X_COMMAND_PARITY_BIT[] = "PARI";

const char HM1X_RESPONSE_OK[] = "OK";
const char HM1X_RESPONSE_GET[] = "+Get:";
const char HM1X_RESPONSE_SET[] = "+Set:";

const char HM1X_OK_CONN_EDR[] = "OK+CONE:";
const char HM1X_OK_CONN_BLE[] = "OK+CONB:";
const char HM1X_OK_DISCON_EDR[] = "OK+LSTE";
const char HM1X_OK_DISCON_BLE[] = "OK+LSTB";

const char HM1X_RESPONSE_PLUS[] = "+";
const char HM1X_QUERY_STRING[] = "?";

const char HM1X_DISCONNECT_RESPONSE_LEN = 20;
const uint8_t HM1X_CONNECT_LENGTH = 20;
const uint8_t HM1X_DISCONNECT_LENGTH = 21;

#ifdef HM1X_I2C_ENABLED
typedef enum {
  I2C_CMD_AVAILABLE, // 0
  I2C_CMD_READ,      // 1
  I2C_CMD_WRITE,     // 2
  I2C_CMD_SET_BAUD,  // 3
  I2C_SET_ADDRESS    // 4
} qwiic_bt_commands_t;
#endif

static const long btBauds[HM1X_BT::NUM_HM1X_BAUDS] = {1200, 2400, 4800, 9600, 19200, 38400, 57600, 115200, 230400};

// These are arrays that maps the required baudChar for each desired baud rate declared in the enum HM1X_baud_t
static const uint8_t btBauds_HM10_11[HM1X_BT::NUM_HM1X_BAUDS] =        {3, 4, 5, 6, 7, 2, 1, 0, 8};
static const uint8_t btBauds_HM16_17_18_19[HM1X_BT::NUM_HM1X_BAUDS] =  {0, 1, 2, 3, 4, 5, 6, 7, 8};
static const uint8_t btBauds_HM12_13[HM1X_BT::NUM_HM1X_BAUDS] =        {0, 2, 3, 4, 5, 6, 7, 8, 0};

static const uint8_t btBauds_validRange_HM10_11[2] =        {0, 8};
static const uint8_t btBauds_validRange_HM16_17_18_19[2] =  {0, 8};
static const uint8_t btBauds_validRange_HM12_13[2] =        {1, 7};

// Class Constructor
HM1X_BT::HM1X_BT(HM1X_model_t btModel)
{
    _btModel = btModel;
    
    _connectedBle = false;
    _connectedEdr = false;
    _edrAddress = "";
    _bleAddress = "";
    _response = "";

    _polling = false;

    // set model-specific variables
    // _isEdrSupported, _btBauds_ptr, and _validBaudBounds_ptr
    setModelSpecificVariables();

#ifdef HM1X_SOFTWARE_SERIAL_ENABLED
    _softSerial = NULL;
#endif
#ifdef HM1X_HARDWARE_SERIAL_ENABLED
    _serialPort = NULL;
#endif
#ifdef HM1X_I2C_ENABLED
    _wirePort = NULL;
    _wireAddress = 0;
#endif
}

#ifdef HM1X_SOFTWARE_SERIAL_ENABLED
boolean HM1X_BT::begin(SoftwareSerial & softSerial, unsigned long baud)
{
    _softSerial = &softSerial;
    _softSerial->begin(baud);
    
#ifdef CHECK_HM1X_CONNECTION_ON_BEGIN
    if( init() == HM1X_SUCCESS ) 
    {
        return true;
    }

    // If init() fails the first time, try forcing the baud rate to the requested baud and try again.
    if (forceBaud(baud) == HM1X_SUCCESS)
    {
        reset();
        _softSerial->begin(baud);
        delay(5000); // Delay long enough for module to reset
        if( init() == HM1X_SUCCESS ) 
        {
            return true;
        }
    }

    return false;
#else
    return true;
#endif
}
#endif

#ifdef HM1X_HARDWARE_SERIAL_ENABLED
boolean HM1X_BT::begin(HardwareSerial &serialPort, unsigned long baud)
{
    _serialPort = &serialPort;
    _serialPort->begin(baud);

#ifdef CHECK_HM1X_CONNECTION_ON_BEGIN
    if( init() == HM1X_SUCCESS ) 
    {
        return true;
    }

    // If init() fails the first time, try forcing the baud rate to the requested baud and try again.
    if (forceBaud(baud) == HM1X_SUCCESS)
    {
        reset();
        _serialPort->begin(baud);
        delay(5000); // Delay long enough for module to reset
        if( init() == HM1X_SUCCESS ) 
        {
            return true;
        }
    }

    return false;
#else
    return true;
#endif
}
#endif

#ifdef HM1X_I2C_ENABLED
boolean HM1X_BT::begin(TwoWire & wirePort, uint8_t wireAddress)
{
    _wirePort = &wirePort;
    _wireAddress = wireAddress;

    _wirePort->begin();

#ifdef CHECK_HM1X_CONNECTION_ON_BEGIN
    //writeI2cBaud(HM1X_BAUD_9600);
    if( init() == HM1X_SUCCESS ) 
    {
        return true;
    }

    // If we fail to initialize, try forcing the baud rate to 9600
    // then iterate through all possible bauds, and send AT+BAUD2 command
    /*for (uint8_t i = HM1X_BAUD_4800; i < NUM_HM1X_BAUDS; i++) 
    {
        writeI2cBaud(i);
        setBaud(HM1X_BAUD_9600);
    }
    // Then try init one more time:
    if( init() == HM1X_SUCCESS ) 
    {
        return true;
    }*/

    return false;
#else
    return true;
#endif
}
#endif

boolean HM1X_BT::setupPoll(void)
{
    HM1X_error_t err;
    err = notify(true, true);

    if (err == HM1X_SUCCESS)
    {
        _polling = true;
        return true;
    }
    return false;
}

boolean HM1X_BT::poll(void)
{
    String response = "";
    boolean handled = false;

    // Receive serial stream, delay for a bit between charaters
    if (hwAvailable())
    {
        while (hwAvailable())
        {
            response += readChar();
            delay(HM1X_POLL_DELAY);
        }
    }

    if (response.length() == 0)
    {
        return false;
    }

    if (response.length() >= 7)
    {
        if (response.substring(0, 7) == "OK+INIT") 
        {
            // TODO: Module restarted -- need to do anything?
            handled = true;
        }
        else if (response.substring(0, strlen(HM1X_OK_CONN_EDR)) == String(HM1X_OK_CONN_EDR))
        {
            _edrAddress = response.substring(8,20);
            _connectedEdr = true;
            handled = true;
        }
        else if (response.substring(0, strlen(HM1X_OK_CONN_BLE)) == String(HM1X_OK_CONN_BLE))
        {
            _bleAddress = response.substring(8,20);
            _connectedBle = true;
            handled = true;
        }
        else if (response.substring(0, strlen(HM1X_OK_DISCON_EDR)) == String(HM1X_OK_DISCON_EDR))
        {
            _edrAddress = response.substring(8, 20);
            _connectedEdr = false;
            handled = true;
        }
        else if (response.substring(0, strlen(HM1X_OK_DISCON_BLE)) == String(HM1X_OK_DISCON_BLE))
        {
            _bleAddress = response.substring(8, 20);
            _connectedBle = false;
            handled = true;
        }
    }
    if (handled == false)
    {
        // Store response into local buffer
        _response += response;
    }
    return handled;
}

int HM1X_BT::available(void)
{
    // If we've polled, then return either _response.length()
    //       or otherwise bytes available in I2C/Serial buffer.
    if ( _polling )
    {
        return _response.length();
    }
    else
    {
        return hwAvailable();
    }

    //return hwAvailable();
}

char HM1X_BT::read(void)
{
    // If we've polled, then return either _response.length()
    //       or otherwise bytes available in I2C/Serial buffer.
    if ( _polling )
    {
        char retVal = _response.charAt(0);
        _response = _response.substring(1);
        return retVal;
    }
    else
    {
        return readChar();
    }
}


size_t HM1X_BT::write(uint8_t c)
{
    if (0)
    {

    }
#ifdef HM1X_SOFTWARE_SERIAL_ENABLED
    else if (_softSerial != NULL)
    {
        return _softSerial->write(c);
    }
#endif
#ifdef HM1X_HARDWARE_SERIAL_ENABLED
    else if (_serialPort != NULL)
    {
        return _serialPort->write(c);
    }
#endif
#ifdef HM1X_I2C_ENABLED
    else if (_wirePort != NULL)
    {
        _wirePort->beginTransmission(_wireAddress);
        _wirePort->write(I2C_CMD_WRITE);
        _wirePort->write(c);
        _wirePort->endTransmission(true);
        return (size_t) 1;
    }
    return (size_t) 0;
#endif
}

size_t HM1X_BT::write(const char *str)
{
    size_t len = strlen(str);

    if (0)
    {

    }
#ifdef HM1X_SOFTWARE_SERIAL_ENABLED
    else if (_softSerial != NULL)
    {
        return _softSerial->print(str);
    }
#endif
#ifdef HM1X_HARDWARE_SERIAL_ENABLED
    else if (_serialPort != NULL)
    {
        return _serialPort->print(str);
    }
#endif
#ifdef HM1X_I2C_ENABLED
    else if (_wirePort != NULL)
    {
        _wirePort->beginTransmission(_wireAddress);
        _wirePort->write(I2C_CMD_WRITE);
        for (int i = 0; i < len; i++)
        {
            _wirePort->write(str[i]);
        }
        _wirePort->endTransmission(true);
        return len;
    }
#endif
    return (size_t) 0;
}

size_t HM1X_BT::write(const char * buffer, size_t size)
{
    if (0)
    {

    }
#ifdef HM1X_SOFTWARE_SERIAL_ENABLED
    else if (_softSerial != NULL)
    {
        return _softSerial->print(buffer);
    }
#endif
#ifdef HM1X_HARDWARE_SERIAL_ENABLED
    else if (_serialPort != NULL)
    {
        return _serialPort->print(buffer);
    }
#endif
#ifdef HM1X_I2C_ENABLED
    else if (_wirePort != NULL)
    {
        _wirePort->beginTransmission(_wireAddress);
        _wirePort->write(I2C_CMD_WRITE);
        for (int i = 0; i < size; i++)
        {
            _wirePort->write(buffer[i]);
        }
        _wirePort->endTransmission(true);
        return size;
    }
#endif
    return (size_t) 0;
}

HM1X_error_t HM1X_BT::testOrDisconnect(void)
{
    char * response;
    HM1X_error_t err;

    response = (char *) calloc(HM1X_DISCONNECT_RESPONSE_LEN + 2, sizeof(char));
    if (response == NULL) return HM1X_OUT_OF_MEMORY;

    sendCommandWithTimeout("", &response, HM1X_DEFAULT_TIMEOUT);

    if (strcmp(response, HM1X_RESPONSE_OK) == 0)
    {
        err = HM1X_SUCCESS;
    }
    else if (strlen(response) == HM1X_DISCONNECT_RESPONSE_LEN)
    {
        // "AT" disconnected us from a BLE source, return success
        // TODO: Could check here to make sure response is "OK+LSTE:001122334455"
        err = HM1X_SUCCESS;
    }
    else
    {
        err = HM1X_SUCCESS; //HM1X_UNEXPECTED_RESPONSE;
    }

    free(response);

    return err;
}

// AT+RENEW -- Restore factory defaults
HM1X_error_t HM1X_BT::factoryDefaults(void)
{
    String expectedResponse;
    char * command;
    char * response;
    HM1X_error_t err;

    // Generate command: "AT+RENEW"
    command = (char *) calloc(strlen(HM1X_COMMAND_FACTORY_DEFAULTS) + 1, sizeof(char));
    if (command == NULL) return HM1X_OUT_OF_MEMORY;
    sprintf(command, "%s", HM1X_COMMAND_FACTORY_DEFAULTS);

    // Generate expected response: "OK+RENEW"
    response = (char *) calloc(strlen(HM1X_RESPONSE_OK) + strlen(HM1X_RESPONSE_PLUS) 
                      + strlen(HM1X_COMMAND_FACTORY_DEFAULTS) + 1, sizeof(char));
    if (response == NULL) return HM1X_OUT_OF_MEMORY;
    sprintf(response, "%s%s%s", HM1X_RESPONSE_OK, HM1X_RESPONSE_PLUS, HM1X_COMMAND_FACTORY_DEFAULTS);

    err = sendCommandWithResponseAndTimeout(command, response, HM1X_DEFAULT_TIMEOUT);

    free(response);
    free(command);

    return err;
}

// AT+RESET -- Restart module
HM1X_error_t HM1X_BT::reset(void)
{
    String expectedResponse;
    char * command;
    char * response;
    HM1X_error_t err;

    // Generate command: "AT+RESET"
    command = (char *) calloc(strlen(HM1X_COMMAND_RESET) + 1, sizeof(char));
    if (command == NULL) return HM1X_OUT_OF_MEMORY;
    sprintf(command, "%s", HM1X_COMMAND_RESET);

    // Generate expected response: "OK+RESET"
    response = (char *) calloc(strlen(HM1X_RESPONSE_OK) + strlen(HM1X_COMMAND_RESET) + 1, sizeof(char));
    if (response == NULL) return HM1X_OUT_OF_MEMORY;
    sprintf(response, "%s%s%s", HM1X_RESPONSE_OK, HM1X_RESPONSE_PLUS, HM1X_COMMAND_RESET);

    err = sendCommandWithResponseAndTimeout(command, response, HM1X_DEFAULT_TIMEOUT);

    free(response);
    free(command);

    return err;
}

// AT+VERR -- Software version
HM1X_error_t HM1X_BT::version(char * version)
{
    String expectedResponse;
    char * command;
    char * response;
    int retNum;

    // Generate command: "AT+VERR?"
    command = (char *) calloc(strlen(HM1X_COMMAND_VERSION) + strlen(HM1X_QUERY_STRING) + 1, sizeof(char));
    if (command == NULL) return HM1X_OUT_OF_MEMORY;
    strcpy(command, HM1X_COMMAND_VERSION);
    strcat(command, HM1X_QUERY_STRING);

    // Generate expected response: "OK+RESET"
    response = (char *) calloc(12 + 1, sizeof(char));
    if (response == NULL) return HM1X_OUT_OF_MEMORY;

    sendCommandWithTimeout(command, &response, HM1X_DEFAULT_TIMEOUT);
    strcpy(version, response + strlen(HM1X_RESPONSE_OK) + strlen(HM1X_RESPONSE_GET));

    free(response);
    free(command);

    return HM1X_SUCCESS;
}

HM1X_error_t HM1X_BT::notifyInfo(boolean enabled)
{
    HM1X_error_t err;
    char * command;
    char * response;
    char param;

    if (enabled) param = '1';
    else param = '0';

    // Build command: e.g. AT+NOTI1
    command = (char *) calloc(strlen(HM1X_COMMAND_NOTIFY_INIT) + 2, sizeof(char));
    if (command == NULL)
    {
        return HM1X_OUT_OF_MEMORY;
    }
    sprintf(command, "%s%c", HM1X_COMMAND_NOTIFY_INIT, param);

    // Build expected response: e.g. OK+Set:1
    response = (char *) calloc(strlen(HM1X_RESPONSE_OK) + strlen(HM1X_RESPONSE_SET) + 2, sizeof(char));
    if (response == NULL)
    {
        free(command);
        return HM1X_OUT_OF_MEMORY;
    }
    sprintf(response, "%s%s%c", HM1X_RESPONSE_OK, HM1X_RESPONSE_SET, param);

    err = sendCommandWithResponseAndTimeout(command, response, HM1X_DEFAULT_TIMEOUT);
    
    free(command);
    free(response);

    return err;
}


// AT+NOTI -- Set notify information 
HM1X_error_t HM1X_BT::notifyMode(boolean enabled)
{
    HM1X_error_t err;
    char * command;
    char * response;
    char param;
    
    if (enabled) param = '1';
    else param = '0';

    // Build command: e.g. AT+NOTP1
    command = (char *) calloc(strlen(HM1X_COMMAND_NOTIFY_MODE) + 2, sizeof(char));
    if (command == NULL)
    {
        return HM1X_OUT_OF_MEMORY;
    }
    sprintf(command, "%s%c", HM1X_COMMAND_NOTIFY_MODE, param);

    // Build expected response: e.g. OK+Set:1)
    response = (char *) calloc(strlen(HM1X_RESPONSE_OK) + strlen(HM1X_RESPONSE_SET) + 2, sizeof(char));
    if (response == NULL)
    {
        free(command);
        return HM1X_OUT_OF_MEMORY;
    }
    sprintf(response, "%s%s%c", HM1X_RESPONSE_OK, HM1X_RESPONSE_SET, param);

    err = sendCommandWithResponseAndTimeout(command, response, HM1X_DEFAULT_TIMEOUT);
    
    free(command);
    free(response);

    return err;
}


// AT+NOTI, AT+NOTP -- Notify information
HM1X_error_t HM1X_BT::notify(boolean enabled, boolean withAddress)
{
    HM1X_error_t err;

    err = notifyMode(withAddress);
    err = notifyInfo(enabled);
    return err;
}

// gets EDR name as return value
// does not support HM-15/16/17/18/19
String HM1X_BT::getEdrName(void)
{
    char * name;
    String retName = "";

    name = (char *)malloc(32* sizeof(char));
    if (name != NULL)
    {
        if (getEdrName(name) == HM1X_SUCCESS)
        {
            retName = String(name);
        }
    }
    return retName;
}

// Set EDR name and copy it to the character array name
// does not support HM-15/16/17/18/19
HM1X_error_t HM1X_BT::getEdrName(char * name)
{
    HM1X_error_t err;
    char * command;
    char * response;
    int retNameLen;

    // Check if EDR is supported
    if ( !_isEdrSupported )
    {
        // return error
        return HM1X_ERROR_ER;
    }

    // Create command string: AT+NAME?
    command = (char *) calloc(strlen(HM1X_COMMAND_EDR_NAME) + strlen(HM1X_QUERY_STRING) + 2, sizeof(char));
    if (command == NULL)
    {
        return HM1X_OUT_OF_MEMORY;
    }
    strcat(command, HM1X_COMMAND_EDR_NAME);
    strcat(command, HM1X_QUERY_STRING);

    // Allocate enough memory for a response (up to 28 bytes + "OK+Get:")
    response = (char *) calloc(strlen(HM1X_RESPONSE_OK) + strlen(HM1X_RESPONSE_GET) + 28 + 2, sizeof(char));
    if (response == NULL)
    {
        return HM1X_OUT_OF_MEMORY;
    }

    retNameLen = sendCommandWithTimeout(command, &response, HM1X_RESPONSE_TIMEOUT);
    if (retNameLen == 0)
    {
        return HM1X_ERROR_TIMEOUT;
    }
    strcpy(name, response + (strlen(HM1X_RESPONSE_OK) + strlen(HM1X_RESPONSE_GET)));

    free(response);
    free(command);
    
    return HM1X_SUCCESS;
}

// Set the EDR device name with the input string parameter
// does not support HM-15/16/17/18/19
HM1X_error_t HM1X_BT::setEdrName(String name)
{
    char * edrChar;

    // Check if EDR is supported
    if ( !_isEdrSupported )
    {
        // return error
        return HM1X_ERROR_ER;
    }

    edrChar = (char *) calloc(name.length() + 1, sizeof(char));
    name.toCharArray(edrChar, name.length() + 1);
    return setEdrName(edrChar);
}

// Set EDR name and copy it to the character array name
HM1X_error_t HM1X_BT::setEdrName(const char * name)
{
    HM1X_error_t err;
    char * command;
    char * response;
    int nameLen;

    // Check if EDR is supported
    if ( !_isEdrSupported )
    {
        // return error
        return HM1X_ERROR_ER;
    }

    nameLen = strlen(name);

    if (nameLen > 28) // Name can't exceed 28 characters
    {
        return HM1X_UNEXPECTED_RESPONSE;
    }

    // Build command: e.g. AT+NAMEMY_EDR_DEVICE
    command = (char *) calloc(strlen(HM1X_COMMAND_EDR_NAME) + nameLen + 2, sizeof(char));
    if (command == NULL)
    {
        return HM1X_OUT_OF_MEMORY;
    }
    strcat(command, HM1X_COMMAND_EDR_NAME);
    strcat(command, name);

    // Build expected response: e.g. OK+Set:MY_EDR_DEVICE
    response = (char *) calloc(strlen(HM1X_RESPONSE_OK) + strlen(HM1X_RESPONSE_SET) + nameLen + 2, sizeof(char));
    if (response == NULL)
    {
        free(command);
        return HM1X_OUT_OF_MEMORY;
    }
    strcat(response, HM1X_RESPONSE_OK);
    strcat(response, HM1X_RESPONSE_SET);
    strcat(response, name);

    err = sendCommandWithResponseAndTimeout(command, response, HM1X_DEFAULT_TIMEOUT);
    
    free(command);
    free(response);

    return err;
}

String HM1X_BT::getBleName(void)
{
    char * name;
    String retName = "";
    name = (char *)malloc(32* sizeof(char));
    if (name != NULL)
    {
        if (getBleName(name) == HM1X_SUCCESS)
        {
            retName = String(name);
        }
    }
    return retName;
}

HM1X_error_t HM1X_BT::getBleName(char * name)
{
    HM1X_error_t err;
    char * command;
    char * response;
    int retNameLen;
    char *command_no_ble;

    command_no_ble = (char*) calloc(strlen(HM1X_COMMAND_BLE_NAME), sizeof(char));
    if (command_no_ble == NULL)
    {
        return HM1X_OUT_OF_MEMORY;
    }
    // check if EDR is disabled
    if ( !_isEdrSupported ){
        // use the code for Edr (main)
        strcpy(command_no_ble, HM1X_COMMAND_EDR_NAME);
    }else{
        // use HM1X_COMMAND_BLE_NAME as is
        strcpy(command_no_ble, HM1X_COMMAND_BLE_NAME);
    }

    // Create command string: AT+NAMB?
    command = (char *) calloc(strlen(command_no_ble) + strlen(HM1X_QUERY_STRING) + 2, sizeof(char));
    if (command == NULL)
    {
        return HM1X_OUT_OF_MEMORY;
    }
    strcat(command, command_no_ble);
    strcat(command, HM1X_QUERY_STRING);

    // Allocate enough memory for a response (up to 28 bytes + "OK+Get:")
    response = (char *) calloc(strlen(HM1X_RESPONSE_OK) + strlen(HM1X_RESPONSE_GET) + 28 + 2, sizeof(char));
    if (response == NULL)
    {
        return HM1X_OUT_OF_MEMORY;
    }

    retNameLen = sendCommandWithTimeout(command, &response, HM1X_RESPONSE_TIMEOUT);
    if (retNameLen == 0)
    {
        return HM1X_ERROR_TIMEOUT;
    }
    strcpy(name, response + (strlen(HM1X_RESPONSE_OK) + strlen(HM1X_RESPONSE_GET)));

    free(response);
    free(command);
    free(command_no_ble);
    
    return HM1X_SUCCESS;
}

HM1X_error_t HM1X_BT::setBleName(String name)
{
    char * bleName;
    bleName = (char *) calloc(name.length() + 1, sizeof(char));
    name.toCharArray(bleName, name.length() + 1);
    return setBleName(bleName);
}

HM1X_error_t HM1X_BT::setBleName(const char * name)
{
    HM1X_error_t err;
    char * command;
    char * response;
    int nameLen;
    char *command_no_ble;

    command_no_ble = (char*) calloc(strlen(HM1X_COMMAND_BLE_NAME), sizeof(char));
    if (command_no_ble == NULL)
    {
        return HM1X_OUT_OF_MEMORY;
    }
    // check if EDR is disabled
    if ( !_isEdrSupported ){
        // use the code for Edr (main)
        strcpy(command_no_ble, HM1X_COMMAND_EDR_NAME);
    }else{
        // use as is
        strcpy(command_no_ble, HM1X_COMMAND_BLE_NAME);
    }

    nameLen = strlen(name);

    if (nameLen > 28) // Name can't exceed 28 characters
    {
        return HM1X_UNEXPECTED_RESPONSE;
    }

    // Build command: e.g. AT+NAMBMY_BLE_DEVICE
    command = (char *) calloc(strlen(command_no_ble) + nameLen + 2, sizeof(char));
    if (command == NULL)
    {
        return HM1X_OUT_OF_MEMORY;
    }
    strcat(command, command_no_ble);
    strcat(command, name);

    // Build expected response: e.g. OK+Set:MY_BLE_DEVICE
    response = (char *) calloc(strlen(HM1X_RESPONSE_OK) + strlen(HM1X_RESPONSE_SET) + nameLen + 2, sizeof(char));
    if (response == NULL)
    {
        free(command);
        return HM1X_OUT_OF_MEMORY;
    }
    strcat(response, HM1X_RESPONSE_OK);
    strcat(response, HM1X_RESPONSE_SET);
    strcat(response, name);

    err = sendCommandWithResponseAndTimeout(command, response, HM1X_DEFAULT_TIMEOUT);
    
    free(command);
    free(response);
    free(command_no_ble);

    return err;
}

// checks EDR address
// does not support HM-15/16/17/18/19
String HM1X_BT::edrAddress(void)
{
    char * address;
    address = (char *) calloc(22, sizeof(char));
    if (edrAddress(address) == HM1X_SUCCESS)
    {
        return String(address);
    }
    return ""; // Return nothing on fail
}

// AT+ADDE -- EDR address
// does not support HM-15/16/17/18/19
HM1X_error_t HM1X_BT::edrAddress(char * retAddress)
{
    char * command;
    char * response;

    if ( (_btModel >= HM15) && (_btModel <= HM19))
    {
        // HM-15 to HM-19 does not support EDR
        // return error
        return HM1X_ERROR_ER;
    }

    command = (char *) calloc(strlen(HM1X_COMMAND_EDR_ADR) + strlen(HM1X_QUERY_STRING) + 1, sizeof(char));
    if (command == NULL) return HM1X_OUT_OF_MEMORY;
    strcpy(command, HM1X_COMMAND_EDR_ADR);
    strcat(command, HM1X_QUERY_STRING);

    response = (char *) calloc(20 + 1, sizeof(char));
    if (response == NULL) return HM1X_OUT_OF_MEMORY;

    sendCommandWithTimeout(command, &response, HM1X_RESPONSE_TIMEOUT);

    strcpy(retAddress, response + (strlen(HM1X_RESPONSE_OK) + strlen(HM1X_RESPONSE_GET)));

    free(response);
    free(command);
    
    return HM1X_SUCCESS;
}

String HM1X_BT::bleAddress(void)
{
    char * address;
    address = (char *) calloc(22, sizeof(char));
    if (bleAddress(address) == HM1X_SUCCESS)
    {
        return String(address);
    }
    return ""; // Return nothing on fail
}

// AT+ADDB -- BLE address
// AT+ADDR for non-dual devices
HM1X_error_t HM1X_BT::bleAddress(char * retAddress)
{
    char * command;
    char * response;
    char *command_no_ble;

    command_no_ble = (char*) calloc(strlen(HM1X_COMMAND_BLE_ADR_SINGLE), sizeof(char));
    if (command_no_ble == NULL)
    {
        return HM1X_OUT_OF_MEMORY;
    }
    // check if EDR is disabled
    if ( !_isEdrSupported ){
        // use the code for Edr (main)
        strcpy(command_no_ble, HM1X_COMMAND_BLE_ADR_SINGLE);
    }else{
        // use as is
        strcpy(command_no_ble, HM1X_COMMAND_BLE_NAME);
    }

    command = (char *) calloc(strlen(command_no_ble) + strlen(HM1X_QUERY_STRING) + 1, sizeof(char));
    if (command == NULL) return HM1X_OUT_OF_MEMORY;
    strcpy(command, command_no_ble);
    strcat(command, HM1X_QUERY_STRING);

    response = (char *) calloc(20 + 1, sizeof(char));
    if (response == NULL) return HM1X_OUT_OF_MEMORY;

    sendCommandWithTimeout(command, &response, HM1X_RESPONSE_TIMEOUT);

    strcpy(retAddress, response + (strlen(HM1X_RESPONSE_OK) + strlen(HM1X_RESPONSE_GET)));
    
    free(response);
    free(command);
    free(command_no_ble);

    return HM1X_SUCCESS;
}

// AT+RADE, AT+RADB -- Last connected EDR/BLE address
// does not support HM-15/16/17/18/19
HM1X_error_t HM1X_BT::lastEdrAddress(char * address)
{
    char * command;
    char * response;

    // Check if EDR is supported
    if ( !_isEdrSupported )
    {
        // return error
        return HM1X_ERROR_ER;
    }

    command = (char *) calloc(strlen(HM1X_COMMAND_LAST_EDR) + strlen(HM1X_QUERY_STRING) + 1, sizeof(char));
    if (command == NULL) return HM1X_OUT_OF_MEMORY;
    strcpy(command, HM1X_COMMAND_LAST_EDR);
    strcat(command, HM1X_QUERY_STRING);

    response = (char *) calloc(20 + 1, sizeof(char));
    if (response == NULL) return HM1X_OUT_OF_MEMORY;

    sendCommandWithTimeout(command, &response, HM1X_RESPONSE_TIMEOUT);

    strcpy(address, response + (strlen(HM1X_RESPONSE_OK) + strlen(HM1X_RESPONSE_GET)));

    free(response);
    free(command);
    
    return HM1X_SUCCESS;
}

HM1X_error_t HM1X_BT::lastBleAddress(char * address)
{
    char * command;
    char * response;
    char *command_no_ble;

    command_no_ble = (char*) calloc(strlen(HM1X_COMMAND_LAST_BLE), sizeof(char));
    if (command_no_ble == NULL)
    {
        return HM1X_OUT_OF_MEMORY;
    }
    // check if EDR is disabled
    if ( !_isEdrSupported ){
        // use the code for Edr (main)
        strcpy(command_no_ble, HM1X_COMMAND_LAST_SINGLE);
    }else{
        // use as is
        strcpy(command_no_ble, HM1X_COMMAND_LAST_BLE);
    }

    command = (char *) calloc(strlen(command_no_ble) + strlen(HM1X_QUERY_STRING) + 1, sizeof(char));
    if (command == NULL) return HM1X_OUT_OF_MEMORY;
    strcpy(command, command_no_ble);
    strcat(command, HM1X_QUERY_STRING);

    response = (char *) calloc(20 + 1, sizeof(char));
    if (response == NULL) return HM1X_OUT_OF_MEMORY;

    sendCommandWithTimeout(command, &response, HM1X_RESPONSE_TIMEOUT);

    strcpy(address, response + (strlen(HM1X_RESPONSE_OK) + strlen(HM1X_RESPONSE_GET)));

    free(response);
    free(command);
    free(command_no_ble);
    
    return HM1X_SUCCESS;
}

// AT+BONDE, AT+BONDB --- Clear EDR/BLE bond info
// does not support HM-15/16/17/18/19
HM1X_error_t HM1X_BT::clearEdrBond(void)
{
    HM1X_error_t err;
    char * command;
    char * response;

    // Check if EDR is supported
    if ( !_isEdrSupported )
    {
        // return error
        return HM1X_ERROR_ER;
    }

    // Build command: e.g. AT+BONDE
    command = (char *) calloc(strlen(HM1X_COMMAND_CLEAR_BOND_EDR) + 1, sizeof(char));
    if (command == NULL)
    {
        return HM1X_OUT_OF_MEMORY;
    }
    strcat(command, HM1X_COMMAND_CLEAR_BOND_EDR);

    // Build expected response: e.g. OK+BONDE
    response = (char *) calloc(strlen(HM1X_RESPONSE_OK) + strlen(HM1X_RESPONSE_PLUS)
                    + strlen(HM1X_COMMAND_CLEAR_BOND_EDR) + 1, sizeof(char));
    if (response == NULL)
    {
        free(command);
        return HM1X_OUT_OF_MEMORY;
    }
    strcat(response, HM1X_RESPONSE_OK);
    strcat(response, HM1X_RESPONSE_PLUS);
    strcat(response, HM1X_COMMAND_CLEAR_BOND_EDR);

    err = sendCommandWithResponseAndTimeout(command, response, HM1X_DEFAULT_TIMEOUT);
    
    free(command);
    free(response);

    return err;
}

HM1X_error_t HM1X_BT::clearBleBond(void)
{
    HM1X_error_t err;
    char * command;
    char * response;

    // Build command: e.g. AT+BONDB
    command = (char *) calloc(strlen(HM1X_COMMAND_CLEAR_BOND_BLE) + 2, sizeof(char));
    if (command == NULL)
    {
        return HM1X_OUT_OF_MEMORY;
    }
    strcat(command, HM1X_COMMAND_CLEAR_BOND_BLE);

    // Build expected response: e.g. OK+BONDB
    response = (char *) calloc(strlen(HM1X_RESPONSE_OK) + strlen(HM1X_RESPONSE_PLUS)
                    + strlen(HM1X_COMMAND_CLEAR_BOND_BLE) + 2, sizeof(char));
    if (response == NULL)
    {
        free(command);
        return HM1X_OUT_OF_MEMORY;
    }
    strcat(response, HM1X_RESPONSE_OK);
    strcat(response, HM1X_RESPONSE_PLUS);
    strcat(response, HM1X_COMMAND_CLEAR_BOND_BLE);

    err = sendCommandWithResponseAndTimeout(command, response, HM1X_DEFAULT_TIMEOUT);
    
    free(command);
    free(response);

    return err;
}

// AT+CLEAE, AT+CLEAB -- Clear last connected EDR/BLE address
// does not support HM-15/16/17/18/19
HM1X_error_t HM1X_BT::clearEdrConnected(void)
{
    HM1X_error_t err;
    char * command;
    char * response;

    // Check if EDR is supported
    if ( !_isEdrSupported )
    {
        // return error
        return HM1X_ERROR_ER;
    }

    // Build command: e.g. AT+CLEAE
    command = (char *) calloc(strlen(HM1X_COMMAND_CLEAR_ADR_EDR) + 1, sizeof(char));
    if (command == NULL)
    {
        return HM1X_OUT_OF_MEMORY;
    }
    strcat(command, HM1X_COMMAND_CLEAR_ADR_EDR);

    // Build expected response: e.g. OK+CLEAE
    response = (char *) calloc(strlen(HM1X_RESPONSE_OK) + strlen(HM1X_RESPONSE_PLUS)
                    + strlen(HM1X_COMMAND_CLEAR_ADR_EDR) + 1, sizeof(char));
    if (response == NULL)
    {
        free(command);
        return HM1X_OUT_OF_MEMORY;
    }
    strcat(response, HM1X_RESPONSE_OK);
    strcat(response, HM1X_RESPONSE_PLUS);
    strcat(response, HM1X_COMMAND_CLEAR_ADR_EDR);

    err = sendCommandWithResponseAndTimeout(command, response, HM1X_DEFAULT_TIMEOUT);
    
    free(command);
    free(response);

    return err;
}

HM1X_error_t HM1X_BT::clearBleConnected(void)
{
    HM1X_error_t err;
    char * command;
    char * response;
    char *command_no_ble;

    command_no_ble = (char*) calloc(strlen(HM1X_COMMAND_CLEAR_ADR_BLE), sizeof(char));
    if (command_no_ble == NULL)
    {
        return HM1X_OUT_OF_MEMORY;
    }
    // check if EDR is disabled
    if ( !_isEdrSupported ){
        // use the code for Edr (main)
        strcpy(command_no_ble, HM1X_COMMAND_CLEAR_ADR_SINGLE);
    }else{
        // use as is
        strcpy(command_no_ble, HM1X_COMMAND_CLEAR_ADR_BLE);
    }

    // Build command: e.g. AT+CLEAB
    command = (char *) calloc(strlen(command_no_ble) + 1, sizeof(char));
    if (command == NULL)
    {
        return HM1X_OUT_OF_MEMORY;
    }
    strcat(command, command_no_ble);

    // Build expected response: e.g. OK+CLEAB
    response = (char *) calloc(strlen(HM1X_RESPONSE_OK) + strlen(HM1X_RESPONSE_PLUS)
                    + strlen(command_no_ble) + 1, sizeof(char));
    if (response == NULL)
    {
        free(command);
        return HM1X_OUT_OF_MEMORY;
    }
    strcat(response, HM1X_RESPONSE_OK);
    strcat(response, HM1X_RESPONSE_PLUS);
    strcat(response, command_no_ble);

    err = sendCommandWithResponseAndTimeout(command, response, HM1X_DEFAULT_TIMEOUT);
    
    free(command);
    free(response);
    free(command_no_ble);

    return err;
}

// AT+ROLE, AT+ROLB -- EDR/BLE mode
// does not support HM-15/16/17/18/19
HM1X_error_t HM1X_BT::getEdrMode(HM1X_edr_mode_t * mode)
{
    char * command;
    char * response;

    // Check if EDR is supported
    if ( !_isEdrSupported )
    {
        // return error
        return HM1X_ERROR_ER;
    }

    // Set command string: "AT+ROLE?""
    command = (char *) calloc(strlen(HM1X_COMMAND_EDR_MODE) + strlen(HM1X_QUERY_STRING) + 1, sizeof(char));
    if (command == NULL) return HM1X_OUT_OF_MEMORY;
    strcpy(command, HM1X_COMMAND_EDR_MODE);
    strcat(command, HM1X_QUERY_STRING);

    response = (char *) calloc(sizeof(HM1X_RESPONSE_OK) + sizeof(HM1X_RESPONSE_GET) + 2, sizeof(char));
    if (response == NULL) return HM1X_OUT_OF_MEMORY;

    sendCommandWithTimeout(command, &response, HM1X_RESPONSE_TIMEOUT);

    strcpy(response, response + (strlen(HM1X_RESPONSE_OK) + strlen(HM1X_RESPONSE_GET)));

    if (strcmp(response, "0") == 0)
    {
        *mode = EDR_SLAVE;
    }
    else if (strcmp(response, "1") == 0)
    {
        *mode = EDR_MASTER;
    }
    else 
    {
        *mode = EDR_MODE_INVALID;
        return HM1X_UNEXPECTED_RESPONSE;
    }

    free(response);
    free(command);
    
    return HM1X_SUCCESS;
}

// sets the EDR mode
// does not support HM-15/16/17/18/19
HM1X_error_t HM1X_BT::setEdrMode(HM1X_edr_mode_t mode)
{
    HM1X_error_t err;
    char * command;
    char * response;
    char modeParam;

    // Check if EDR is supported
    if ( !_isEdrSupported )
    {
        // return error
        return HM1X_ERROR_ER;
    }
    
    if (mode == EDR_MODE_INVALID)
    {
        return HM1X_UNEXPECTED_RESPONSE;
    }

    // Build command: e.g. AT+ROLE0
    command = (char *) calloc(strlen(HM1X_COMMAND_EDR_MODE) + 2, sizeof(char));
    modeParam = (mode == EDR_SLAVE) ? '0' : '1';
    sprintf(command, "%s%c", HM1X_COMMAND_EDR_MODE, modeParam);

    // Build expected response: e.g. OK+Set:0
    response = (char *) calloc(strlen(HM1X_RESPONSE_OK) + strlen(HM1X_RESPONSE_SET) + 2, sizeof(char));
    if (response == NULL)
    {
        free(command);
        return HM1X_OUT_OF_MEMORY;
    }
    sprintf(response, "%s%s%c", HM1X_RESPONSE_OK, HM1X_RESPONSE_SET, modeParam);

    err = sendCommandWithResponseAndTimeout(command, response, HM1X_DEFAULT_TIMEOUT);
    
    free(command);
    free(response);

    return err;
}

HM1X_error_t HM1X_BT::getBleMode(HM1X_ble_mode_t * mode)
{
    char * command;
    char * response;
    char *command_no_ble;

    command_no_ble = (char*) calloc(strlen(HM1X_COMMAND_BLE_MODE), sizeof(char));
    if (command_no_ble == NULL)
    {
        return HM1X_OUT_OF_MEMORY;
    }
    // check if EDR is disabled
    if ( !_isEdrSupported ){
        // use the code for Edr (main)
        strcpy(command_no_ble, HM1X_COMMAND_EDR_MODE);
    }else{
        // use as is
        strcpy(command_no_ble, HM1X_COMMAND_BLE_MODE);
    }

    // Set command string: "AT+ROLE?""
    command = (char *) calloc(strlen(command_no_ble) + strlen(HM1X_QUERY_STRING) + 1, sizeof(char));
    if (command == NULL) return HM1X_OUT_OF_MEMORY;
    strcpy(command, command_no_ble);
    strcat(command, HM1X_QUERY_STRING);

    response = (char *) calloc(sizeof(HM1X_RESPONSE_OK) + sizeof(HM1X_RESPONSE_GET) + 2, sizeof(char));
    if (response == NULL) return HM1X_OUT_OF_MEMORY;

    sendCommandWithTimeout(command, &response, HM1X_RESPONSE_TIMEOUT);

    strcpy(response, response + (strlen(HM1X_RESPONSE_OK) + strlen(HM1X_RESPONSE_GET)));

    if (strcmp(response, "0") == 0)
    {
        *mode = BLE_PERIPHERAL;
    }
    else if (strcmp(response, "1") == 0)
    {
        *mode = BLE_CENTRAL;
    }
    else 
    {
        *mode = BLE_MODE_INVALID;
        return HM1X_UNEXPECTED_RESPONSE;
    }

    free(response);
    free(command);
    free(command_no_ble);
    
    return HM1X_SUCCESS;
}

HM1X_error_t HM1X_BT::setBleMode(HM1X_ble_mode_t mode)
{
    HM1X_error_t err;
    char * command;
    char * response;
    char modeParam;
    char *command_no_ble;
    
    if (mode == EDR_MODE_INVALID)
    {
        return HM1X_UNEXPECTED_RESPONSE;
    }

    command_no_ble = (char*) calloc(strlen(HM1X_COMMAND_BLE_MODE), sizeof(char));
    if (command_no_ble == NULL)
    {
        return HM1X_OUT_OF_MEMORY;
    }
    // check if EDR is disabled
    if ( !_isEdrSupported ){
        // use the code for Edr (main)
        strcpy(command_no_ble, HM1X_COMMAND_EDR_MODE);
    }else{
        // use as is
        strcpy(command_no_ble, HM1X_COMMAND_BLE_MODE);
    }

    // Build command: e.g. AT+ROLB0
    command = (char *) calloc(strlen(command_no_ble) + 2, sizeof(char));
    modeParam = (mode == EDR_SLAVE) ? '0' : '1';
    sprintf(command, "%s%c", command_no_ble, modeParam);

    // Build expected response: e.g. OK+Set:0
    response = (char *) calloc(strlen(HM1X_RESPONSE_OK) + strlen(HM1X_RESPONSE_SET) + 2, sizeof(char));
    if (response == NULL)
    {
        free(command);
        return HM1X_OUT_OF_MEMORY;
    }
    sprintf(response, "%s%s%c", HM1X_RESPONSE_OK, HM1X_RESPONSE_SET, modeParam);

    err = sendCommandWithResponseAndTimeout(command, response, HM1X_DEFAULT_TIMEOUT);
    
    free(command);
    free(response);
    free(command_no_ble);

    return err;
}

// AT+HIGH -- Data transmission speed mode
// Disabled: SPP and BLE speeds balanced
// Enabled: SPP will go high speed
HM1X_error_t HM1X_BT::enableHighSpeedSPP(boolean enabled)
{
    HM1X_error_t err;
    char * command;
    char * response;
    char hsParam;

    // Build command: e.g. AT+HIGH0
    command = (char *) calloc(strlen(HM1X_COMMAND_HIGH_SPEED_SPP) + 2, sizeof(char));
    hsParam = (enabled) ? '1' : '0';
    sprintf(command, "%s%c", HM1X_COMMAND_HIGH_SPEED_SPP, hsParam);

    // Build expected response: e.g. OK+Set:0
    response = (char *) calloc(strlen(HM1X_RESPONSE_OK) + strlen(HM1X_RESPONSE_SET) + 2, sizeof(char));
    if (response == NULL)
    {
        free(command);
        return HM1X_OUT_OF_MEMORY;
    }
    sprintf(response, "%s%s%c", HM1X_RESPONSE_OK, HM1X_RESPONSE_SET, hsParam);

    err = sendCommandWithResponseAndTimeout(command, response, HM1X_DEFAULT_TIMEOUT);
    
    free(command);
    free(response);

    return err;
}

// This is only supported in HM12/13/14
HM1X_error_t HM1X_BT::enableDualMode(boolean enabled)
{
    HM1X_error_t err;
    char * command;
    char * response;
    char hsParam;

    // Check if EDR is supported
    if ( !_isEdrSupported )
    {
        // return error
        return HM1X_ERROR_ER;
    }

    // Build command: e.g. AT+DUAL0
    command = (char *) calloc(strlen(HM1X_COMMAND_DUAL_WORK_MODE) + 2, sizeof(char));
    hsParam = (enabled) ? '0' : '1';
    sprintf(command, "%s%c", HM1X_COMMAND_DUAL_WORK_MODE, hsParam);

    // Build expected response: e.g. OK+Set:0
    response = (char *) calloc(strlen(HM1X_RESPONSE_OK) + strlen(HM1X_RESPONSE_SET) + 2, sizeof(char));
    if (response == NULL)
    {
        free(command);
        return HM1X_OUT_OF_MEMORY;
    }
    sprintf(response, "%s%s%c", HM1X_RESPONSE_OK, HM1X_RESPONSE_SET, hsParam);

    err = sendCommandWithResponseAndTimeout(command, response, HM1X_DEFAULT_TIMEOUT);
    
    free(command);
    free(response);

    return err;
}

// AT+MODE -- Module work mode
// Enabled: Allow remote control (send AT commands remotely)
// Disabled: Data transmission only 
HM1X_error_t HM1X_BT::enableRemoteControl(boolean enabled)
{
    HM1X_error_t err;
    char * command;
    char * response;
    char param;

    // Build command: e.g. AT+MODE0
    command = (char *) calloc(strlen(HM1X_COMMAND_MODULE_WORK_MODE) + 2, sizeof(char));
    if (command == NULL) return HM1X_OUT_OF_MEMORY;
    param = (enabled) ? '1' : '0';
    sprintf(command, "%s%c", HM1X_COMMAND_MODULE_WORK_MODE, param);

    // Build expected response: e.g. OK+Set:0
    response = (char *) calloc(strlen(HM1X_RESPONSE_OK) + strlen(HM1X_RESPONSE_SET) + 2, sizeof(char));
    if (response == NULL)
    {
        free(command);
        return HM1X_OUT_OF_MEMORY;
    }
    sprintf(response, "%s%s%c", HM1X_RESPONSE_OK, HM1X_RESPONSE_SET, param);

    err = sendCommandWithResponseAndTimeout(command, response, HM1X_DEFAULT_TIMEOUT);
    
    free(command);
    free(response);

    return err;
}

// AT+ATOB -- A to B mode
// When two modules connected (BLE and SPP), this will route data from one to the other
HM1X_error_t HM1X_BT::enableAtoB(boolean enable)
{
    HM1X_error_t err;
    char * command;
    char * response;
    char param;

    // Build command: e.g. AT+ATOB0
    command = (char *) calloc(strlen(HM1X_COMMAND_A_TO_B_MODE) + 2, sizeof(char));
    if (command == NULL) return HM1X_OUT_OF_MEMORY;
    param = (enable) ? '1' : '0';
    sprintf(command, "%s%c", HM1X_COMMAND_A_TO_B_MODE, param);

    // Build expected response: e.g. OK+Set:0
    response = (char *) calloc(strlen(HM1X_RESPONSE_OK) + strlen(HM1X_RESPONSE_SET) + 2, sizeof(char));
    if (response == NULL)
    {
        free(command);
        return HM1X_OUT_OF_MEMORY;
    }
    sprintf(response, "%s%s%c", HM1X_RESPONSE_OK, HM1X_RESPONSE_SET, param);
    
    err = sendCommandWithResponseAndTimeout(command, response, HM1X_DEFAULT_TIMEOUT);
    
    free(command);
    free(response);

    return err;
}

    // AT+AUTH -- Authentication mode
HM1X_error_t HM1X_BT::enableAuthenticationMode(boolean enable)
{
    HM1X_error_t err;
    char * command;
    char * response;
    char param;

    // Build command: e.g. AT+ATOB0
    command = (char *) calloc(strlen(HM1X_COMMAND_AUTHENTICATION_MODE) + 2, sizeof(char));
    if (command == NULL) return HM1X_OUT_OF_MEMORY;
    param = (enable) ? '1' : '0';
    sprintf(command, "%s%c", HM1X_COMMAND_AUTHENTICATION_MODE, param);

    // Build expected response: e.g. OK+Set:0
    response = (char *) calloc(strlen(HM1X_RESPONSE_OK) + strlen(HM1X_RESPONSE_SET) + 2, sizeof(char));
    if (response == NULL)
    {
        free(command);
        return HM1X_OUT_OF_MEMORY;
    }
    sprintf(response, "%s%s%c", HM1X_RESPONSE_OK, HM1X_RESPONSE_SET, param);

    err = sendCommandWithResponseAndTimeout(command, response, HM1X_DEFAULT_TIMEOUT);
    
    free(command);
    free(response);

    return err;
}

// AT+PINE, AT+PINB -- EDR/BLE PIN Code
// does not support HM-15/16/17/18/19
HM1X_error_t HM1X_BT::getEdrPin(char * code)
{
    char * command;
    char * response;

    // Check if EDR is supported
    if ( !_isEdrSupported )
    {
        // return error
        return HM1X_ERROR_ER;
    }

    // Set command string: "AT+PINE?""
    command = (char *) calloc(strlen(HM1X_COMMAND_EDR_PIN_CODE) + strlen(HM1X_QUERY_STRING) + 1, sizeof(char));
    if (command == NULL) return HM1X_OUT_OF_MEMORY;
    sprintf(command, "%s%s", HM1X_COMMAND_EDR_PIN_CODE, HM1X_QUERY_STRING);

    response = (char *) calloc(sizeof(HM1X_RESPONSE_OK) + sizeof(HM1X_RESPONSE_GET) + 8, sizeof(char));
    if (response == NULL) return HM1X_OUT_OF_MEMORY;
    
    sendCommandWithTimeout(command, &response, HM1X_RESPONSE_TIMEOUT);

    strcpy(code, response + (strlen(HM1X_RESPONSE_OK) + strlen(HM1X_RESPONSE_GET)));

    free(response);
    free(command);
    
    return HM1X_SUCCESS;    
}

// AT+PINE, AT+PINB -- EDR/BLE PIN Code
HM1X_error_t HM1X_BT::getBlePin(char * code)
{
    char * command;
    char * response;
    char *command_no_ble;

    command_no_ble = (char*) calloc(strlen(HM1X_COMMAND_BLE_PIN_CODE), sizeof(char));
    if (command_no_ble == NULL)
    {
        return HM1X_OUT_OF_MEMORY;
    }
    // check if EDR is disabled
    if ( !_isEdrSupported ){
        // use the code for single
        strcpy(command_no_ble, HM1X_COMMAND_PIN_CODE_SINGLE);
    }else{
        // use as is
        strcpy(command_no_ble, HM1X_COMMAND_BLE_PIN_CODE);
    }

    // Set command string: "AT+PINB?""
    command = (char *) calloc(strlen(command_no_ble) + strlen(HM1X_QUERY_STRING) + 1, sizeof(char));
    if (command == NULL) return HM1X_OUT_OF_MEMORY;
    sprintf(command, "%s%s", command_no_ble, HM1X_QUERY_STRING);

    response = (char *) calloc(sizeof(HM1X_RESPONSE_OK) + sizeof(HM1X_RESPONSE_GET) + 8, sizeof(char));
    if (response == NULL) return HM1X_OUT_OF_MEMORY;
    
    sendCommandWithTimeout(command, &response, HM1X_RESPONSE_TIMEOUT);

    strcpy(code, response + (strlen(HM1X_RESPONSE_OK) + strlen(HM1X_RESPONSE_GET)));

    free(response);
    free(command);
    free(command_no_ble);
    
    return HM1X_SUCCESS;    
}

HM1X_error_t HM1X_BT::setEdrPin(char * code)
{
    HM1X_error_t err;
    char * command;
    char * response;

    if (strlen(code) > 6) return HM1X_UNEXPECTED_RESPONSE;
    // TODO: Should check if the code is numeric here

    // Build command: e.g. AT+EDR1234
    command = (char *) calloc(strlen(HM1X_COMMAND_EDR_PIN_CODE) + 8, sizeof(char));
    if (command == NULL) return HM1X_OUT_OF_MEMORY;
    sprintf(command, "%s%s", HM1X_COMMAND_EDR_PIN_CODE, code);

    // Build expected response: e.g. OK+Set:1234
    response = (char *) calloc(strlen(HM1X_RESPONSE_OK) + strlen(HM1X_RESPONSE_SET) + 8, sizeof(char));
    if (response == NULL)
    {
        free(command);
        return HM1X_OUT_OF_MEMORY;
    }
    sprintf(response, "%s%s%s", HM1X_RESPONSE_OK, HM1X_RESPONSE_SET, code);

    err = sendCommandWithResponseAndTimeout(command, response, HM1X_DEFAULT_TIMEOUT);
    
    free(command);
    free(response);

    return err;
}

HM1X_error_t HM1X_BT::setBlePin(char * code)
{
    HM1X_error_t err;
    char * command;
    char * response;
    char *command_no_ble;

    command_no_ble = (char*) calloc(strlen(HM1X_COMMAND_BLE_PIN_CODE), sizeof(char));
    if (command_no_ble == NULL)
    {
        return HM1X_OUT_OF_MEMORY;
    }
    // check if EDR is disabled
    if ( !_isEdrSupported ){
        // use the code for single
        strcpy(command_no_ble, HM1X_COMMAND_PIN_CODE_SINGLE);
    }else{
        // use as is
        strcpy(command_no_ble, HM1X_COMMAND_BLE_PIN_CODE);
    }
    

    if (strlen(code) > 6) return HM1X_UNEXPECTED_RESPONSE;
    // TODO: Should check if the code is numeric here

    // Build command: e.g. AT+EDR1234
    command = (char *) calloc(strlen(command_no_ble) + 8, sizeof(char));
    if (command == NULL) return HM1X_OUT_OF_MEMORY;
    sprintf(command, "%s%s", command_no_ble, code);

    // Build expected response: e.g. OK+Set:1234
    response = (char *) calloc(strlen(HM1X_RESPONSE_OK) + strlen(HM1X_RESPONSE_SET) + 8, sizeof(char));
    if (response == NULL)
    {
        free(command);
        return HM1X_OUT_OF_MEMORY;
    }
    sprintf(response, "%s%s%s", HM1X_RESPONSE_OK, HM1X_RESPONSE_SET, code);

    err = sendCommandWithResponseAndTimeout(command, response, HM1X_DEFAULT_TIMEOUT);
    
    free(command);
    free(response);
    free(command_no_ble);

    return err;
}

// AT+COFD -- Class of device
// Can set to any value between 0x000000 to 0xFFFFFE
HM1X_error_t HM1X_BT::setCod(uint32_t cod)
{
    HM1X_error_t err;
    char * command;
    char * response;

    // Build command: e.g. AT+COFD001F00
    command =(char *) calloc(strlen(HM1X_COMMAND_COD) + 7, sizeof(char));
    sprintf(command, "%s%06X", HM1X_COMMAND_COD, cod);

    // Build expected response: e.g. OK+Set:0
    response = (char *) calloc(strlen(HM1X_RESPONSE_OK) + strlen(HM1X_RESPONSE_SET) + 7, sizeof(char));
    if (response == NULL)
    {
        free(command);
        return HM1X_OUT_OF_MEMORY;
    }
    sprintf(response, "%s%s%06X", HM1X_RESPONSE_OK, HM1X_RESPONSE_SET, cod);

    err = sendCommandWithResponseAndTimeout(command, response, HM1X_DEFAULT_TIMEOUT);
    
    free(command);
    free(response);

    return err;
}

// AT+COUP -- Update connection parameter
// Only usable in  BLE slave mode. Updates min/max interval, slave latency, and connection supervised timeout
HM1X_error_t HM1X_BT::enableUpdateConnectionParameter(boolean enable)
{
    HM1X_error_t err;
    char * command;
    char * response;
    char param;

    // Build command: e.g. AT+COUP1
    command = (char *) calloc(strlen(HM1X_COMMAND_UPDATE_CON_PARAM) + 2, sizeof(char));
    if (command == NULL) return HM1X_OUT_OF_MEMORY;
    param = (enable) ? '1' : '0';
    sprintf(command, "%s%c", HM1X_COMMAND_UPDATE_CON_PARAM, param);

    // Build expected response: e.g. OK+Set:0
    response = (char *) calloc(strlen(HM1X_RESPONSE_OK) + strlen(HM1X_RESPONSE_SET) + 2, sizeof(char));
    if (response == NULL)
    {
        free(command);
        return HM1X_OUT_OF_MEMORY;
    }
    sprintf(response, "%s%s%c", HM1X_RESPONSE_OK, HM1X_RESPONSE_SET, param);

    err = sendCommandWithResponseAndTimeout(command, response, HM1X_DEFAULT_TIMEOUT);
    
    free(command);
    free(response);

    return err;
}

boolean HM1X_BT::iBeacon(boolean enable)
{
    if (enableiBeacon(enable) == HM1X_SUCCESS)
    {
        return true;
    }
    return false;
}

// AT+IBEA -- Enable iBeacon
HM1X_error_t HM1X_BT::enableiBeacon(boolean enabled)
{
    HM1X_error_t err;
    char * command;
    char * response;
    char param;

    // Build command: e.g. AT+IBEA1
    command = (char *) calloc(strlen(HM1X_COMMAND_IBEACON_SWITCH) + 2, sizeof(char));
    if (command == NULL) return HM1X_OUT_OF_MEMORY;
    param = (enabled) ? '1' : '0';
    sprintf(command, "%s%c", HM1X_COMMAND_IBEACON_SWITCH, param);

    // Build expected response: e.g. OK+Set:1
    response = (char *) calloc(strlen(HM1X_RESPONSE_OK) + strlen(HM1X_RESPONSE_SET) + 2, sizeof(char));
    if (response == NULL)
    {
        free(command);
        return HM1X_OUT_OF_MEMORY;
    }
    sprintf(response, "%s%s%c", HM1X_RESPONSE_OK, HM1X_RESPONSE_SET, param);

    err = sendCommandWithResponseAndTimeout(command, response, HM1X_DEFAULT_TIMEOUT);
    
    free(command);
    free(response);

    return err;
}

String HM1X_BT::getiBeaconUUID(void)
{
    char * uuid;
    uuid = (char *) calloc(33, sizeof(char));
    if (getiBeaconUUID(uuid) == HM1X_SUCCESS)
    {
        return String(uuid);
    }
    return "";
}

// AT+IBE0, AT+IBE1, AT+IBE2, AT+IBE3 -- Get/Set iBeacon UUID
HM1X_error_t HM1X_BT::getiBeaconUUID(char * uuid)
{
    HM1X_error_t err;
    char * uuidPart;
    uuidPart = (char *) calloc(8 * 4, sizeof(char));
    
    if (uuidPart == NULL) {
        return HM1X_OUT_OF_MEMORY;
    }

    for (int i = 0; i < 4; i++)
    {
        err = getiBeaconUUID(uuidPart + (i * 8), i);
        if (err != HM1X_SUCCESS)
        {
            free(uuidPart);
            return err;
        }
    }
    strcpy(uuid, uuidPart);
    return HM1X_SUCCESS;
}

HM1X_error_t HM1X_BT::getiBeaconUUID(char * uuid, uint8_t position)
{
    char * command;
    char * response;

    if (position > 3)
    {
        return HM1X_UNEXPECTED_RESPONSE;
    }

    // Set command string: "AT+IBE<pos>?""
    command = (char *) calloc(strlen(HM1X_COMMAND_IBEACON_UUID) + strlen(HM1X_QUERY_STRING) + 2, sizeof(char));
    if (command == NULL) return HM1X_OUT_OF_MEMORY;
    sprintf(command, "%s%d%s", HM1X_COMMAND_IBEACON_UUID, position, HM1X_QUERY_STRING);

    response = (char *) calloc(sizeof(HM1X_RESPONSE_OK) + sizeof(HM1X_RESPONSE_GET) + 10, sizeof(char));
    if (response == NULL) return HM1X_OUT_OF_MEMORY;
    
    sendCommandWithTimeout(command, &response, HM1X_RESPONSE_TIMEOUT);

    strcpy(uuid, response + (strlen(HM1X_RESPONSE_OK) + strlen(HM1X_RESPONSE_GET)));

    free(response);
    free(command);
    
    return HM1X_SUCCESS;
}

HM1X_error_t HM1X_BT::setiBeaconUUID(char * first, char * second, char * third, char * fourth)
{
    setiBeaconUUID(first, 0);
    setiBeaconUUID(second, 1);
    setiBeaconUUID(third, 2);
    return setiBeaconUUID(fourth, 3);
}

HM1X_error_t HM1X_BT::setiBeaconUUID(char * uuid, uint8_t position)
{
    HM1X_error_t err;
    char * command;
    char * response;
    char param;

    if (position > 3)
    {
        return HM1X_UNEXPECTED_RESPONSE;
    }

    if (strlen(uuid) < 8)
    {
        // TODO: Should pre-pend 0's here
        return HM1X_UNEXPECTED_RESPONSE;
    }
    else if (strlen(uuid) > 8 )
    {
        return HM1X_UNEXPECTED_RESPONSE;
    }

    // Build command: e.g. AT+IBEA1
    command = (char *) calloc(strlen(HM1X_COMMAND_IBEACON_UUID) + 10, sizeof(char));
    if (command == NULL) return HM1X_OUT_OF_MEMORY;
    sprintf(command, "%s%d%s", HM1X_COMMAND_IBEACON_UUID, position, uuid);

    // Build expected response: e.g. OK+Set:1
    response = (char *) calloc(strlen(HM1X_RESPONSE_OK) + strlen(HM1X_RESPONSE_SET) + 10, sizeof(char));
    if (response == NULL)
    {
        free(command);
        return HM1X_OUT_OF_MEMORY;
    }
    sprintf(response, "%s%s%s", HM1X_RESPONSE_OK, HM1X_RESPONSE_SET, uuid);

    err = sendCommandWithResponseAndTimeout(command, response, HM1X_DEFAULT_TIMEOUT);

    
    free(command);
    free(response);

    return err;
}

// AT+MAJO, AT+MINO -- iBeacon Major version
/*HM1X_error_t HM1X_BT::getiBeaconVersions(uint16_t * major, uint16_t * minor)
{
    getiBeaconMajor(major);
    return getiBeaconMinor(minor);
}

HM1X_error_t HM1X_BT::setiBeaconVersions(uint16_t major, uint16_t minor)
{
    setiBeaconMajor(major);
    return setiBeaconMinor(minor);
}*/

HM1X_error_t HM1X_BT::getiBeaconMajor(uint16_t * version)
{
    char * command;
    char * response;
    char * hexMajor;

    // Set command string: "AT+MAJO?""
    command = (char *) calloc(strlen(HM1X_COMMAND_IBEACON_MAJOR) + strlen(HM1X_QUERY_STRING) + 2, sizeof(char));
    if (command == NULL) return HM1X_OUT_OF_MEMORY;
    sprintf(command, "%s%s", HM1X_COMMAND_IBEACON_MAJOR, HM1X_QUERY_STRING);

    response = (char *) calloc(sizeof(HM1X_RESPONSE_OK) + sizeof(HM1X_RESPONSE_GET) + 6, sizeof(char));
    if (response == NULL) return HM1X_OUT_OF_MEMORY;

    hexMajor = (char *) malloc(4 * sizeof(char) + 1);
    if (hexMajor == NULL) return HM1X_OUT_OF_MEMORY;
    
    sendCommandWithTimeout(command, &response, HM1X_RESPONSE_TIMEOUT);

    strcpy(hexMajor, response + (strlen(HM1X_RESPONSE_OK) + strlen(HM1X_RESPONSE_GET)));
    *version = strtol(hexMajor, NULL, 16);

    free(response);
    free(command);
    
    return HM1X_SUCCESS;
}

HM1X_error_t HM1X_BT::setiBeaconMajor(uint16_t version)
{
    HM1X_error_t err;
    char * command;
    char * response;

    if (version > 0xFFFE) 
    {
        return HM1X_UNEXPECTED_RESPONSE;
    }

    // Build command: e.g. AT+MAJOR0001
    command = (char *) calloc(strlen(HM1X_COMMAND_IBEACON_MAJOR) + 6, sizeof(char));
    if (command == NULL) return HM1X_OUT_OF_MEMORY;
    sprintf(command, "%s%04X", HM1X_COMMAND_IBEACON_MAJOR, version);

    // Build expected response: e.g. OK+Set:0001
    response = (char *) calloc(strlen(HM1X_RESPONSE_OK) + strlen(HM1X_RESPONSE_SET) + 6, sizeof(char));
    if (response == NULL)
    {
        free(command);
        return HM1X_OUT_OF_MEMORY;
    }
    sprintf(response, "%s%s%04X", HM1X_RESPONSE_OK, HM1X_RESPONSE_SET, version);

    err = sendCommandWithResponseAndTimeout(command, response, HM1X_DEFAULT_TIMEOUT);
    
    free(command);
    free(response);

    return err;
}

HM1X_error_t HM1X_BT::getiBeaconMinor(uint16_t * version)
{
    char * command;
    char * response;
    char * hexMinor;

    // Set command string: "AT+MAJO?""
    command = (char *) calloc(strlen(HM1X_COMMAND_IBEACON_MINOR) + strlen(HM1X_QUERY_STRING) + 2, sizeof(char));
    if (command == NULL) return HM1X_OUT_OF_MEMORY;
    sprintf(command, "%s%s", HM1X_COMMAND_IBEACON_MINOR, HM1X_QUERY_STRING);

    response = (char *) calloc(sizeof(HM1X_RESPONSE_OK) + sizeof(HM1X_RESPONSE_GET) + 6, sizeof(char));
    if (response == NULL) return HM1X_OUT_OF_MEMORY;

    hexMinor = (char *) malloc(4 * sizeof(char) + 1);
    if (hexMinor == NULL) return HM1X_OUT_OF_MEMORY;
    
    sendCommandWithTimeout(command, &response, HM1X_RESPONSE_TIMEOUT);

    strcpy(hexMinor, response + (strlen(HM1X_RESPONSE_OK) + strlen(HM1X_RESPONSE_GET)));
    *version = strtol(hexMinor, NULL, 16);

    free(response);
    free(command);
    
    return HM1X_SUCCESS;
}

HM1X_error_t HM1X_BT::setiBeaconMinor(uint16_t version)
{
    HM1X_error_t err;
    char * command;
    char * response;

    if (version > 0xFFFE) 
    {
        return HM1X_UNEXPECTED_RESPONSE;
    }

    // Build command: e.g. AT+MAJOR0001
    command = (char *) calloc(strlen(HM1X_COMMAND_IBEACON_MINOR) + 6, sizeof(char));
    if (command == NULL) return HM1X_OUT_OF_MEMORY;
    sprintf(command, "%s%04X", HM1X_COMMAND_IBEACON_MINOR, version);

    // Build expected response: e.g. OK+Set:0001
    response = (char *) calloc(strlen(HM1X_RESPONSE_OK) + strlen(HM1X_RESPONSE_SET) + 6, sizeof(char));
    if (response == NULL)
    {
        free(command);
        return HM1X_OUT_OF_MEMORY;
    }
    sprintf(response, "%s%s%04X", HM1X_RESPONSE_OK, HM1X_RESPONSE_SET, version);

    err = sendCommandWithResponseAndTimeout(command, response, HM1X_DEFAULT_TIMEOUT);
    
    free(command);
    free(response);

    return err;
}

// AT+MEAS -- iBeacon Measured Power
HM1X_error_t HM1X_BT::getiBeaconPower(uint8_t * power)
{
    char * command;
    char * response;
    char * hexPower;

    // Set command string: "AT+MEAS?""
    command = (char *) calloc(strlen(HM1X_COMMAND_IBEACON_POWER) + strlen(HM1X_QUERY_STRING) + 2, sizeof(char));
    if (command == NULL) return HM1X_OUT_OF_MEMORY;
    sprintf(command, "%s%s", HM1X_COMMAND_IBEACON_POWER, HM1X_QUERY_STRING);

    response = (char *) calloc(sizeof(HM1X_RESPONSE_OK) + sizeof(HM1X_RESPONSE_GET) + 4, sizeof(char));
    if (response == NULL) return HM1X_OUT_OF_MEMORY;

    sendCommandWithTimeout(command, &response, HM1X_RESPONSE_TIMEOUT);
    
    hexPower = (char *) malloc(2 * sizeof(char) + 1);
    if (hexPower == NULL) return HM1X_OUT_OF_MEMORY;

    strcpy(hexPower, response + (strlen(HM1X_RESPONSE_OK) + strlen(HM1X_RESPONSE_GET)));
    *power = strtol(hexPower, NULL, 16);

    free(response);
    free(command);
    
    return HM1X_SUCCESS;
}

HM1X_error_t HM1X_BT::setiBeaconPower(uint8_t power)
{
    char * command;
    char * response;
    HM1X_error_t err;

    if (power > 255) return HM1X_UNEXPECTED_RESPONSE;

    // Build command: e.g. AT+MEASFF
    command = (char *) calloc(strlen(HM1X_COMMAND_IBEACON_POWER) + 6, sizeof(char));
    if (command == NULL) return HM1X_OUT_OF_MEMORY;
    sprintf(command, "%s%2X", HM1X_COMMAND_IBEACON_POWER, power);

    // Build expected response: e.g. OK+Set:FF
    response = (char *) calloc(strlen(HM1X_RESPONSE_OK) + strlen(HM1X_RESPONSE_SET) + 6, sizeof(char));
    if (response == NULL)
    {
        free(command);
        return HM1X_OUT_OF_MEMORY;
    }
    sprintf(response, "%s%s%2X", HM1X_RESPONSE_OK, HM1X_RESPONSE_SET, power);
    
    err = sendCommandWithResponseAndTimeout(command, response, HM1X_DEFAULT_TIMEOUT);
    
    free(command);
    free(response);

    return err;
}

// AT+MTUS -- MTU Size
HM1X_error_t HM1X_BT::setMtuSize(HM1X_mtu_size_t mtuSize)
{
    char * command;
    char * response;
    char param;
    HM1X_error_t err;
    
    if (mtuSize == MTU_SIZE_60)
    {
        param = '0';
    }
    else if (mtuSize == MTU_SIZE_120)
    {
        param = '1';
    }
    else return HM1X_UNEXPECTED_RESPONSE;

    // Build command: e.g. AT+MTUS0
    command = (char *) calloc(strlen(HM1X_COMMAND_MTU_SIZE) + 6, sizeof(char));
    if (command == NULL) return HM1X_OUT_OF_MEMORY;
    sprintf(command, "%s%c", HM1X_COMMAND_MTU_SIZE, param);

    // Build expected response: e.g. OK+Set:0
    response = (char *) calloc(strlen(HM1X_RESPONSE_OK) + strlen(HM1X_RESPONSE_SET) + 2, sizeof(char));
    if (response == NULL)
    {
        free(command);
        return HM1X_OUT_OF_MEMORY;
    }
    sprintf(response, "%s%s%c", HM1X_RESPONSE_OK, HM1X_RESPONSE_SET, param);
    
    err = sendCommandWithResponseAndTimeout(command, response, HM1X_DEFAULT_TIMEOUT);
    
    free(command);
    free(response);

    return err;
}

// AT+SCAN -- EDR Advert type
HM1X_error_t HM1X_BT::getEdrAdvertType(HM1X_edr_advert_t * type)
{
    char * command;
    char * response;

    // Set command string: "AT+SCAN?""
    command = (char *) calloc(strlen(HM1X_COMMAND_ADVERT_TYPE) + strlen(HM1X_QUERY_STRING) + 2, sizeof(char));
    if (command == NULL) return HM1X_OUT_OF_MEMORY;
    sprintf(command, "%s%s", HM1X_COMMAND_ADVERT_TYPE, HM1X_QUERY_STRING);

    response = (char *) calloc(sizeof(HM1X_RESPONSE_OK) + sizeof(HM1X_RESPONSE_GET) + 2, sizeof(char));
    if (response == NULL) return HM1X_OUT_OF_MEMORY;

    sendCommandWithTimeout(command, &response, HM1X_RESPONSE_TIMEOUT);

    strcpy(response, response + (strlen(HM1X_RESPONSE_OK) + strlen(HM1X_RESPONSE_GET)));
    if (strcmp(response, "0") == 0)
    {
        *type = DISCOVERY_AND_CONNECTABLE;
    }
    else if (strcmp(response, "1") == 0)
    {
        *type = ONLY_CONNECTABLE;
    }
    else
    {
        *type = EDR_ADVERT_UNDEFINED;
    }

    free(response);
    free(command);
    
    return HM1X_SUCCESS;
}

HM1X_error_t HM1X_BT::setEdrAdvertType(HM1X_edr_advert_t type)
{
    char * command;
    char * response;
    char param;
    HM1X_error_t err;
    
    if (type == DISCOVERY_AND_CONNECTABLE)
    {
        param = '0';
    }
    else if (type == ONLY_CONNECTABLE)
    {
        param = '1';
    }
    else return HM1X_UNEXPECTED_RESPONSE;

    // Build command: e.g. AT+SCAN0
    command = (char *) calloc(strlen(HM1X_COMMAND_ADVERT_TYPE) + 6, sizeof(char));
    if (command == NULL) return HM1X_OUT_OF_MEMORY;
    sprintf(command, "%s%c", HM1X_COMMAND_ADVERT_TYPE, param);

    // Build expected response: e.g. OK+Set:0
    response = (char *) calloc(strlen(HM1X_RESPONSE_OK) + strlen(HM1X_RESPONSE_SET) + 2, sizeof(char));
    if (response == NULL)
    {
        free(command);
        return HM1X_OUT_OF_MEMORY;
    }
    sprintf(response, "%s%s%c", HM1X_RESPONSE_OK, HM1X_RESPONSE_SET, param);
    
    err = sendCommandWithResponseAndTimeout(command, response, HM1X_DEFAULT_TIMEOUT);
    
    free(command);
    free(response);

    return err;
}

// AT+SAFE -- Module safe mode
HM1X_error_t HM1X_BT::enableSafeMode(boolean enabled)
{
    char * command;
    char * response;
    char param;
    HM1X_error_t err;
    
    if (enabled)
    {
        param = '1';
    }
    else
    {
        param = '0';
    }

    // Build command: e.g. AT+SAFE0
    command = (char *) calloc(strlen(HM1X_COMMAND_SAFE_MODE) + 2, sizeof(char));
    if (command == NULL) return HM1X_OUT_OF_MEMORY;
    sprintf(command, "%s%c", HM1X_COMMAND_SAFE_MODE, param);

    // Build expected response: e.g. OK+Set:0
    response = (char *) calloc(strlen(HM1X_RESPONSE_OK) + strlen(HM1X_RESPONSE_SET) + 2, sizeof(char));
    if (response == NULL)
    {
        free(command);
        return HM1X_OUT_OF_MEMORY;
    }
    sprintf(response, "%s%s%c", HM1X_RESPONSE_OK, HM1X_RESPONSE_SET, param);
    
    err = sendCommandWithResponseAndTimeout(command, response, HM1X_DEFAULT_TIMEOUT);
    
    free(command);
    free(response);

    return err;
}

// AT+ONEM -- Whether to use BLE MAC address
// Note: If you want to use BLE in Android, don't use this command :S
HM1X_error_t HM1X_BT::disableBleAddress(boolean disabled)
{
    char * command;
    char * response;
    char param;
    HM1X_error_t err;
    
    if (disabled)
    {
        param = '1';
    }
    else
    {
        param = '0';
    }

    // Build command: e.g. AT+ONEM0
    command = (char *) calloc(strlen(HM1X_COMMAND_BLE_MAC) + 2, sizeof(char));
    if (command == NULL) return HM1X_OUT_OF_MEMORY;
    sprintf(command, "%s%c", HM1X_COMMAND_BLE_MAC, param);

    // Build expected response: e.g. OK+Set:0
    response = (char *) calloc(strlen(HM1X_RESPONSE_OK) + strlen(HM1X_RESPONSE_SET) + 2, sizeof(char));
    if (response == NULL)
    {
        free(command);
        return HM1X_OUT_OF_MEMORY;
    }
    sprintf(response, "%s%s%c", HM1X_RESPONSE_OK, HM1X_RESPONSE_SET, param);
    
    err = sendCommandWithResponseAndTimeout(command, response, HM1X_DEFAULT_TIMEOUT);
    
    free(command);
    free(response);

    return err;
}

// AT+PIO0 -- Enable system key function on PIO0
HM1X_error_t HM1X_BT::enableSystemKey(boolean enabled)
{
    char * command;
    char * response;
    char param;
    HM1X_error_t err;
    
    if (enabled)
    {
        param = '1';
    }
    else
    {
        param = '0';
    }

    // Build command: e.g. AT+PIO01
    command = (char *) calloc(strlen(HM1X_COMMAND_SYSTEM_KEY) + 2, sizeof(char));
    if (command == NULL) return HM1X_OUT_OF_MEMORY;
    sprintf(command, "%s%c", HM1X_COMMAND_SYSTEM_KEY, param);

    // Build expected response: e.g. OK+Set:1
    response = (char *) calloc(strlen(HM1X_RESPONSE_OK) + strlen(HM1X_RESPONSE_SET) + 2, sizeof(char));
    if (response == NULL)
    {
        free(command);
        return HM1X_OUT_OF_MEMORY;
    }
    sprintf(response, "%s%s%c", HM1X_RESPONSE_OK, HM1X_RESPONSE_SET, param);
    
    err = sendCommandWithResponseAndTimeout(command, response, HM1X_DEFAULT_TIMEOUT);
    
    free(command);
    free(response);

    return err;
}

// AT+POIO1 -- System LED, PIO1 control
HM1X_error_t HM1X_BT::setLedMode(HM1X_led_mode_t mode)
{
    char * command;
    char * response;
    char param;
    HM1X_error_t err;
    
    if (mode == BLINK_DISCONNECTED)
    {
        param = '0';
    }
    else
    {
        param = '1';
    }

    // Build command: e.g. AT+PIO11
    command = (char *) calloc(strlen(HM1X_COMMAND_SYSTEM_LED) + 2, sizeof(char));
    if (command == NULL) return HM1X_OUT_OF_MEMORY;
    sprintf(command, "%s%c", HM1X_COMMAND_SYSTEM_LED, param);

    // Build expected response: e.g. OK+Set:1
    response = (char *) calloc(strlen(HM1X_RESPONSE_OK) + strlen(HM1X_RESPONSE_SET) + 2, sizeof(char));
    if (response == NULL)
    {
        free(command);
        return HM1X_OUT_OF_MEMORY;
    }
    sprintf(response, "%s%s%c", HM1X_RESPONSE_OK, HM1X_RESPONSE_SET, param);
    
    err = sendCommandWithResponseAndTimeout(command, response, HM1X_DEFAULT_TIMEOUT);
    
    free(command);
    free(response);

    return err;
}

// AT+PIO -- Write/query PIO
HM1X_error_t HM1X_BT::readPio(uint8_t pin, uint8_t * value)
{
    char * command;
    char * response;

    if ((pin != 2) && (pin != 3))
    {
        return HM1X_UNEXPECTED_RESPONSE;
    }

    // Set command string: "AT+PIO21""
    command = (char *) calloc(strlen(HM1X_COMMAND_PIO_STATUS) + strlen(HM1X_QUERY_STRING) + 3, sizeof(char));
    if (command == NULL) return HM1X_OUT_OF_MEMORY;
    sprintf(command, "%s%d%s", HM1X_COMMAND_PIO_STATUS, pin, HM1X_QUERY_STRING);

    response = (char *) calloc(sizeof(HM1X_RESPONSE_OK) + sizeof(HM1X_RESPONSE_GET) + 2, sizeof(char));
    if (response == NULL) return HM1X_OUT_OF_MEMORY;

    sendCommandWithTimeout(command, &response, HM1X_RESPONSE_TIMEOUT);

    strcpy(response, response + (strlen(HM1X_RESPONSE_OK) + strlen(HM1X_RESPONSE_GET)));
    
    if (strcmp(response, "0") == 0)
    {
        *value = 0;
    }
    if (strcmp(response, "1") == 0)
    {
        *value = 1;
    }
    else
    {
        return HM1X_UNEXPECTED_RESPONSE;
    }

    free(response);
    free(command);
    
    return HM1X_SUCCESS;
}

HM1X_error_t HM1X_BT::writePio(uint8_t pin, uint8_t value)
{
    char * command;
    char * response;
    HM1X_error_t err;
    uint8_t writeVal = value;

    if ((pin != 2) && (pin != 3))
    {
        return HM1X_UNEXPECTED_RESPONSE;
    }
    if (writeVal >= 1) writeVal = 1;

    // Build command: e.g. AT+PIO21
    command = (char *) calloc(strlen(HM1X_COMMAND_PIO_STATUS) + 4, sizeof(char));
    if (command == NULL) return HM1X_OUT_OF_MEMORY;
    sprintf(command, "%s%d%d", HM1X_COMMAND_PIO_STATUS, pin, writeVal);

    // Build expected response: e.g. OK+Set:11
    response = (char *) calloc(strlen(HM1X_RESPONSE_OK) + strlen(HM1X_RESPONSE_SET) + 2, sizeof(char));
    if (response == NULL)
    {
        free(command);
        return HM1X_OUT_OF_MEMORY;
    }
    sprintf(response, "%s%s%d", HM1X_RESPONSE_OK, HM1X_RESPONSE_SET, value);
    
    err = sendCommandWithResponseAndTimeout(command, response, HM1X_DEFAULT_TIMEOUT);
    
    free(command);
    free(response);

    return err;
}

HM1X_error_t HM1X_BT::setBaud(HM1X_baud_t atob)
{
    HM1X_error_t err;
    char * command;
    char * response;
    char * hsParam;
    char * baudChar = (char*)"0";
    uint8_t baudCharNum;

    command = (char *) calloc(strlen(HM1X_COMMAND_BAUD) + 2, sizeof(char));
    if (command == NULL) return HM1X_OUT_OF_MEMORY;

    // Build command: e.g. AT+BAUD2
    // Handle cases according to model
    if (findBaudFromArray(atob, baudCharNum) != HM1X_SUCCESS){

        return HM1X_UNEXPECTED_RESPONSE;
    
    }

    baudChar[0] = (char)'0' + baudCharNum;

    strcpy(command, HM1X_COMMAND_BAUD);
    strcat(command, baudChar);

    // Build expected response: e.g. OK+Set:2
    response = (char *) calloc(strlen(HM1X_RESPONSE_OK) + strlen(HM1X_RESPONSE_SET) + 2, sizeof(char));
    if (response == NULL)
    {
        free(command);
        return HM1X_OUT_OF_MEMORY;
    }
    strcat(response, HM1X_RESPONSE_OK);
    strcat(response, HM1X_RESPONSE_SET);
    strcat(response, baudChar);
    
    err = sendCommandWithResponseAndTimeout(command, response, HM1X_DEFAULT_TIMEOUT);
    
    free(command);
    free(response);

    return err;
}

/////////////
// Private //
/////////////

// returns the integer that corresponds to the correct baud rate depending on the model
HM1X_error_t HM1X_BT::findBaudFromArray(HM1X_baud_t atob, uint8_t &num){
    
    // check bounds
    if ((atob < _validBaudBounds_ptr[0]) || (atob > _validBaudBounds_ptr[1]))
    {
        // out of bounds
        return HM1X_UNEXPECTED_RESPONSE;
    }

    // look for the index that corresponds to atob
    for( uint8_t i = _validBaudBounds_ptr[0]; i <= _validBaudBounds_ptr[1]; ++i ){
        if (atob == *(_btBauds_ptr + i))
        {
            num = i;
            return HM1X_SUCCESS;
        }
    }

    // invalid atob for the particular model
    return HM1X_UNEXPECTED_RESPONSE;
    
}

HM1X_error_t HM1X_BT::init(void)
{
    HM1X_error_t err;  

    err = testOrDisconnect();
    if (err != HM1X_SUCCESS)
    {
        delay(500); // AT may only disconnecte and not return "OK" response
        err = testOrDisconnect();
        if (err != HM1X_SUCCESS) 
        {
            return err;
        }
    }

    err = notify(false, false);

    return err;
}

HM1X_error_t HM1X_BT::sendCommandWithResponseAndTimeout(const char * command, char * expectedResponse, uint16_t commandTimeout)
{
    unsigned long timeIn = millis();
    char * response;

    sendCommand(command);

    // Wait until we've receved the requested number of characters
    while (hwAvailable() < strlen(expectedResponse))
    {
        if (millis() > timeIn + commandTimeout)
        {
            return HM1X_ERROR_TIMEOUT;
        }
    }
    response = (char *) calloc(hwAvailable() + 1, sizeof(char));
    if (response == NULL)
    {
        return HM1X_OUT_OF_MEMORY;
    }
    readAvailable(response);
    
    // Check for expected response
    if (strcmp(response, expectedResponse) == 0)
    {
        return HM1X_SUCCESS;
    }
    else
    {
        return HM1X_UNEXPECTED_RESPONSE;
    }
}

int HM1X_BT::sendCommandWithTimeout(const char * command, char ** responseDest, uint16_t commandTimeout)
{
    unsigned long timeIn = millis();
    char * response;
    int retVal = 0;

    sendCommand(command);

    // Wait for timeout to occur
    // TODO: Should this also check for an overflow of the serial buffer?
    while (millis() < timeIn + commandTimeout)
        ;

    // Read data into the response string
    // TODO: Isn't it on the calling function to allocate memory for this read?
    response = (char *) calloc(hwAvailable() + 1, sizeof(char));
    if (response == NULL)
    {
        return 0;
    }
    retVal = readAvailable(response);
    strcpy(*responseDest, response);
    return retVal;
}

boolean HM1X_BT::sendCommand(const char * command)
{
    if (strlen(command) == 0) // If string is not null it will include "+"
    {
        hwPrint(HM1X_COMMAND_AT);
        return true;
    }
    else
    {
        char * commandToSend;
        commandToSend = (char *) calloc(strlen(HM1X_COMMAND_AT) + strlen(command) + 2, sizeof(char));
        if (commandToSend == NULL)
        {
            return false;
        }
        strcpy(commandToSend, HM1X_COMMAND_AT);
        strcat(commandToSend, "+");
        strcat(commandToSend, command);

        hwPrint(commandToSend);
        free(commandToSend);
    }

    return true;
}

/*void HM1X_BT::hwFlush(void)
{
    readAvailable();
}*/

size_t HM1X_BT::hwPrint(const char * s)
{
    if (0)
    {

    }
#ifdef HM1X_SOFTWARE_SERIAL_ENABLED
    else if (_softSerial != NULL)
    {
        return _softSerial->print(s);
    }
#endif
#ifdef HM1X_HARDWARE_SERIAL_ENABLED
    else if (_serialPort != NULL)
    {
        return _serialPort->print(s);
    }
#endif
#ifdef HM1X_I2C_ENABLED
    else if (_wirePort != NULL)
    {
        int charsToWrite = strlen(s);
        int charsWritten = 0;
        // ATtiny85 can only read/write 14 bytes at a time via I2C
        // Need to split >14 writes into multiple transmissions
        while (charsToWrite > 0)
        {
            int toWrite;
            if (charsToWrite > 14) toWrite = 14;
            else toWrite = charsToWrite;
            
            _wirePort->beginTransmission(_wireAddress);
            _wirePort->write(I2C_CMD_WRITE);
            for (int i = 0; i < toWrite; i++)
            {
                _wirePort->write(s[i + charsWritten]);
            }
            _wirePort->endTransmission(true);
            charsToWrite -= toWrite;
            charsWritten += toWrite;
        }
        return charsWritten;
    }
#endif
    return 0;
}

int HM1X_BT::readAvailable(char * inString)
{
    int len = 0;
    if (0)
    {

    }
#ifdef HM1X_SOFTWARE_SERIAL_ENABLED
    else if (_softSerial != NULL)
    {
        while (_softSerial->available())
        {
            char c = (char)_softSerial->read();
            inString[len++] = c;
        }
        inString[len] = 0;
    }
#endif
#ifdef HM1X_HARDWARE_SERIAL_ENABLED
    else if (_serialPort != NULL)
    {
        while (_serialPort->available())
        {
            char c = (char)_serialPort->read();
            inString[len++] = c;
        }
        inString[len] = 0;
    }
#endif
#ifdef HM1X_I2C_ENABLED
    else if (_wirePort != NULL)
    {
        int avail = hwAvailable();
        int bytesToRead;
        // I2C on the Tiny can only write out 14(?) bytes at a time.
        // If there are more than 14 bytes to read, loop through
        while (avail > 0)
        {
            if (avail > 14) bytesToRead = 14;
            else bytesToRead = avail;

            _wirePort->beginTransmission(_wireAddress);
            _wirePort->write(I2C_CMD_READ);
            _wirePort->write(bytesToRead);
            _wirePort->endTransmission(false);
            _wirePort->requestFrom(_wireAddress, (uint8_t)bytesToRead);
            for (int i = 0; i < bytesToRead; i++)
            {
                char c = (char) _wirePort->read();
                inString[len++] = c;
            }
            avail -= bytesToRead;
        }
        inString[len] = 0;
    }
#endif

    return len;
}

char HM1X_BT::readChar(void)
{
    char ret;

    if (0)
    {

    }
#ifdef HM1X_SOFTWARE_SERIAL_ENABLED
    else if (_softSerial != NULL)
    {
        ret = (char)_softSerial->read();
    }
#endif
#ifdef HM1X_HARDWARE_SERIAL_ENABLED
    else if (_serialPort != NULL)
    {
        ret = (char)_serialPort->read();
    }
#endif
#ifdef HM1X_I2C_ENABLED
    else if (_wirePort != NULL)
    {
        int avail = hwAvailable();

        _wirePort->beginTransmission(_wireAddress);
        _wirePort->write(I2C_CMD_READ);
        _wirePort->write(1);
        _wirePort->endTransmission(false);
        _wirePort->requestFrom(_wireAddress, (uint8_t)avail);
        ret = (char) _wirePort->read();
    }
#endif

    return ret;
}

int HM1X_BT::hwAvailable(void)
{
    if (0)
    {

    }
#ifdef HM1X_SOFTWARE_SERIAL_ENABLED
    else if (_softSerial != NULL)
    {
        return _softSerial->available();
    }
#endif
#ifdef HM1X_HARDWARE_SERIAL_ENABLED
    else if (_serialPort != NULL) 
    {
        return _serialPort->available();
    }
#endif
#ifdef HM1X_I2C_ENABLED
    else if (_wirePort != NULL)
    {
        _wirePort->beginTransmission(_wireAddress);
        _wirePort->write(I2C_CMD_AVAILABLE);
        _wirePort->endTransmission(false);
        _wirePort->requestFrom(_wireAddress, (uint8_t)1);
        int ret = _wirePort->read();
        return ret;
    }
#endif
    return -1;
}

#ifdef HM1X_I2C_ENABLED
void HM1X_BT::writeI2cBaud(uint8_t baudIndex)
{
    _wirePort->beginTransmission(_wireAddress);
    _wirePort->write(I2C_CMD_SET_BAUD);
    _wirePort->write(baudIndex);
    _wirePort->endTransmission(true);
}
#endif

#ifdef HM1X_I2C_ENABLED
void HM1X_BT::setI2cAddress(uint8_t address)
{
    if ((address < 0x08) || (address > 0x77)) {
        return;
    }
    _wirePort->beginTransmission(_wireAddress);
    _wirePort->write(I2C_SET_ADDRESS);
    _wirePort->write(address);
    _wirePort->endTransmission(true);

}
#endif

HM1X_error_t HM1X_BT::forceBaud(unsigned long baud)
{
    HM1X_error_t err;
    switch (baud) {
    case 1200:
        err = forceBaud(HM1X_BAUD_1200);
        break;
    case 2400:
        err = forceBaud(HM1X_BAUD_2400);
        break;
    case 4800:
        err = forceBaud(HM1X_BAUD_4800);
        break;
    case 9600:
        err = forceBaud(HM1X_BAUD_9600);
        break;
    case 19200:
        err = forceBaud(HM1X_BAUD_19200);
        break;
    case 38400:
        err = forceBaud(HM1X_BAUD_38400);
        break;
    case 57600:
        err = forceBaud(HM1X_BAUD_57600);
        break;
    case 115200:
        err = forceBaud(HM1X_BAUD_115200);
        break;
    case 230400:
        err = forceBaud(HM1X_BAUD_230400);
        break;
    default:
        // Return error on unsupported baud
        err = HM1X_ERROR_ER;
    }

    return err;
}

// sweeps for all possible baud rates then force-set to the indicated baud rate
HM1X_error_t HM1X_BT::forceBaud(HM1X_baud_t baud)
{    
    HM1X_error_t err;
    uint8_t idx;

    for (uint8_t i = _validBaudBounds_ptr[0]; i < _validBaudBounds_ptr[1]; i++) 
    {
        idx = _btBauds_ptr[i];
        if (0)
        {
            
        }
#ifdef HM1X_SOFTWARE_SERIAL_ENABLED
        else if (_softSerial != NULL)
        {
            _softSerial->begin(btBauds[idx]);
        }
#endif
#ifdef HM1X_I2C_ENABLED
        else if (_wirePort != NULL)
        {
            writeI2cBaud(i);
        }
#endif
#ifdef HM1X_HARDWARE_SERIAL_ENABLED
        else if (_serialPort != NULL)
        {
            _serialPort->begin(btBauds[idx]);
        }
#endif
        err = setBaud(baud);
        if (err == HM1X_SUCCESS)
        {
            break; // On success, break out of loop. we found our baud!
        }
    }
    return err;
}

// set model-specific variables
// _isEdrSupported, _btBauds_ptr, and _validBaudBounds_ptr
void HM1X_BT::setModelSpecificVariables(void){

    switch(_btModel)
    {
        case HM10:
        case HM11:
            // HM-10/11: only allow P0~8
            _isEdrSupported = false;
            _validBaudBounds_ptr = &btBauds_validRange_HM10_11[0];
            _btBauds_ptr = &btBauds_HM10_11[0];
            break;
        case HM12:
        case HM13:
            // HM-12/13
            _isEdrSupported = true;
            _validBaudBounds_ptr = &btBauds_validRange_HM12_13[0];
            _btBauds_ptr = &btBauds_HM12_13[0];
            break;
        case HM14:
        case HM15:
        case HM16:
        case HM17:
        case HM18:
        case HM19:
            // HM-14 to 19
            _isEdrSupported = false;
            _validBaudBounds_ptr = &btBauds_validRange_HM16_17_18_19[0];
            _btBauds_ptr = &btBauds_HM16_17_18_19[0];
            break;
    }
}