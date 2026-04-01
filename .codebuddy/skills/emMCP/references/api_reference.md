# emMCP API Reference

## Core Functions

### emMCP_Init

```c
int emMCP_Init(emMCP_t *emMCP);
```

Initialize the emMCP library.

**Parameters:**
- `emMCP` - Pointer to emMCP device structure

**Returns:**
- `0` - Success
- `-1` - Failure (NULL pointer)

**Example:**
```c
static emMCP_t emMCP_dev;
if (emMCP_Init(&emMCP_dev) != 0) {
    // Handle error
}
```

---

### emMCP_TickHandle

```c
void emMCP_TickHandle(int delay_ms);
```

Main processing function - must be called periodically.

**Parameters:**
- `delay_ms` - Delay time in milliseconds for internal timing

**Example:**
```c
while(1) {
    emMCP_TickHandle(10);  // Call every 10ms
}
```

---

### emMCP_AddToolToToolList

```c
int emMCP_AddToolToToolList(emMCP_tool_t *tool);
```

Add a custom MCP tool to the tool list.

**Parameters:**
- `tool` - Pointer to tool structure

**Returns:**
- `0` - Success
- `-32604` - Failure

---

### emMCP_RegistrationTools

```c
int emMCP_RegistrationTools(void);
```

Register all added tools to the AI device.

**Returns:**
- `0` - Success
- `-1` - Failure

**Note:** Must be called after all tools are added.

---

### emMCP_GetParam

```c
cJSON *emMCP_GetParam(cJSON *params, char *param_name);
```

Get a parameter from the JSON params object (used in callbacks).

**Parameters:**
- `params` - JSON params object
- `param_name` - Parameter name to retrieve

**Returns:**
- cJSON object pointer or NULL if not found

---

## Device Control Functions

### emMCP_SetAiWakeUp

```c
int emMCP_SetAiWakeUp(uint8_t WakeUp_Time);
```

Wake up the AI device for a specified duration.

**Parameters:**
- `WakeUp_Time` - Wake duration in seconds

**Returns:**
- `0` - Success
- `-1` - Failure

---

### emMCP_SetAiVolume

```c
int emMCP_SetAiVolume(uint8_t volume);
```

Set the AI device volume.

**Parameters:**
- `volume` - Volume level (0-100)

**Returns:**
- `0` - Success
- `-1` - Failure

---

### emMCP_CheckAiVolume

```c
uint8_t emMCP_CheckAiVolume(void);
```

Get the current AI device volume.

**Returns:**
- Current volume (0-100) or `-1` on error

---

### emMCP_SetBaudrate

```c
int emMCP_SetBaudrate(uint16_t baudrate);
```

Set the UART communication baudrate.

**Parameters:**
- `baudrate` - Baudrate value (e.g., 115200, 921600)

**Returns:**
- `0` - Success
- `-1` - Failure

---

### emMCP_ResponseValue

```c
int emMCP_ResponseValue(char *value);
```

Send a response value to the AI device (call from tool handlers).

**Parameters:**
- `value` - Response value string, or use `emMCP_CTRL_OK`/`emMCP_CTRL_ERROR`

**Returns:**
- `0` - Success
- `-1` - Failure

---

## Event Types

| Event | Value | Description |
|-------|-------|-------------|
| emMCP_EVENT_NONE | 0 | No event |
| emMCP_EVENT_CMD_OK | 1 | Command OK |
| emMCP_EVENT_CMD_ERROR | 2 | Command error |
| emMCP_EVENT_AI_START | 3 | AI started |
| emMCP_EVENT_AI_NETCFG | 4 | Network config mode |
| emMCP_EVENT_AI_NETERR | 5 | Network error |
| emMCP_EVENT_AI_WIFI_CONNNECT | 6 | WiFi connecting |
| emMCP_EVENT_AI_WIFI_CONNECTED | 7 | WiFi connected |
| emMCP_EVENT_AI_WIFI_GOT_IP | 8 | Got IP address |
| emMCP_EVENT_AI_WIFI_DISCONNECT | 9 | WiFi disconnected |
| emMCP_EVENT_AI_WAKE | 10 | Device woke up |
| emMCP_EVENT_AI_SLEEP | 11 | Device sleeping |
| emMCP_EVENT_AI_OTAUPDATE | 12 | OTA updating |
| emMCP_EVENT_AI_OTAOK | 13 | OTA success |
| emMCP_EVENT_AI_OTAERR | 14 | OTA error |
| emMCP_EVENT_AI_MCP_CMD | 15 | MCP command |
| emMCP_EVENT_AI_MCP_Text | 16 | MCP text message |
