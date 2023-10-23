/* Automatically generated file; DO NOT EDIT */
/* Espressif IoT Development Framework Linker Script */
/* Generated from: /home/janhavi/esp/esp-idf/components/esp32/ld/esp32.project.ld.in */

/*  Default entry point:  */
ENTRY(call_start_cpu0);

SECTIONS
{
  /* RTC fast memory holds RTC wake stub code,
     including from any source file named rtc_wake_stub*.c
  */
  .rtc.text :
  {
    . = ALIGN(4);

    *(EXCLUDE_FILE(*libsoc.a:uart_hal_iram.* *libsoc.a:twai_hal_iram.*) .rtc.literal EXCLUDE_FILE(*libsoc.a:uart_hal_iram.* *libsoc.a:twai_hal_iram.*) .rtc.text EXCLUDE_FILE(*libsoc.a:uart_hal_iram.* *libsoc.a:twai_hal_iram.*) .rtc.text.*)
    *libsoc.a:twai_hal_iram.*( .rtc.literal  .rtc.text  .rtc.text.*)
    *libsoc.a:uart_hal_iram.*( .rtc.literal  .rtc.text  .rtc.text.*)

    *rtc_wake_stub*.*(.literal .text .literal.* .text.*)
    _rtc_text_end = ABSOLUTE(.);
  } > rtc_iram_seg

  /*
    This section is required to skip rtc.text area because rtc_iram_seg and
    rtc_data_seg are reflect the same address space on different buses.
  */
  .rtc.dummy :
  {
    _rtc_dummy_start = ABSOLUTE(.);
    _rtc_fast_start = ABSOLUTE(.);
    . = SIZEOF(.rtc.text);
    _rtc_dummy_end = ABSOLUTE(.);
  } > rtc_data_seg

  /* This section located in RTC FAST Memory area.
     It holds data marked with RTC_FAST_ATTR attribute.
     See the file "esp_attr.h" for more information.
  */
  .rtc.force_fast :
  {
    . = ALIGN(4);
    _rtc_force_fast_start = ABSOLUTE(.);
    *(.rtc.force_fast .rtc.force_fast.*)
    . = ALIGN(4) ;
    _rtc_force_fast_end = ABSOLUTE(.);
  } > rtc_data_seg

  /* RTC data section holds RTC wake stub
     data/rodata, including from any source file
     named rtc_wake_stub*.c and the data marked with
     RTC_DATA_ATTR, RTC_RODATA_ATTR attributes.
     The memory location of the data is dependent on
     CONFIG_ESP32_RTCDATA_IN_FAST_MEM option.
  */
  .rtc.data :
  {
    _rtc_data_start = ABSOLUTE(.);

    *(EXCLUDE_FILE(*libsoc.a:uart_hal_iram.* *libsoc.a:twai_hal_iram.*) .rtc.data EXCLUDE_FILE(*libsoc.a:uart_hal_iram.* *libsoc.a:twai_hal_iram.*) .rtc.data.* EXCLUDE_FILE(*libsoc.a:uart_hal_iram.* *libsoc.a:twai_hal_iram.*) .rtc.rodata EXCLUDE_FILE(*libsoc.a:uart_hal_iram.* *libsoc.a:twai_hal_iram.*) .rtc.rodata.*)
    *libsoc.a:twai_hal_iram.*( .rtc.data  .rtc.data.*  .rtc.rodata  .rtc.rodata.*)
    *libsoc.a:uart_hal_iram.*( .rtc.data  .rtc.data.*  .rtc.rodata  .rtc.rodata.*)

    *rtc_wake_stub*.*(.data .rodata .data.* .rodata.* .bss .bss.*)
    _rtc_data_end = ABSOLUTE(.);
  } > rtc_data_location

  /* RTC bss, from any source file named rtc_wake_stub*.c */
  .rtc.bss (NOLOAD) :
  {
    _rtc_bss_start = ABSOLUTE(.);
    *rtc_wake_stub*.*(.bss .bss.*)
    *rtc_wake_stub*.*(COMMON)

    *(EXCLUDE_FILE(*libsoc.a:uart_hal_iram.* *libsoc.a:twai_hal_iram.*) .rtc.bss)
    *libsoc.a:twai_hal_iram.*( .rtc.bss)
    *libsoc.a:uart_hal_iram.*( .rtc.bss)

    _rtc_bss_end = ABSOLUTE(.);
  } > rtc_data_location

  /* This section holds data that should not be initialized at power up
     and will be retained during deep sleep.
     User data marked with RTC_NOINIT_ATTR will be placed
     into this section. See the file "esp_attr.h" for more information.
	 The memory location of the data is dependent on
     CONFIG_ESP32_RTCDATA_IN_FAST_MEM option.
  */
  .rtc_noinit (NOLOAD):
  {
    . = ALIGN(4);
    _rtc_noinit_start = ABSOLUTE(.);
    *(.rtc_noinit .rtc_noinit.*)
    . = ALIGN(4) ;
    _rtc_noinit_end = ABSOLUTE(.);
  } > rtc_data_location

  /* This section located in RTC SLOW Memory area.
     It holds data marked with RTC_SLOW_ATTR attribute.
     See the file "esp_attr.h" for more information.
  */
  .rtc.force_slow :
  {
    . = ALIGN(4);
    _rtc_force_slow_start = ABSOLUTE(.);
    *(.rtc.force_slow .rtc.force_slow.*)
    . = ALIGN(4) ;
    _rtc_force_slow_end = ABSOLUTE(.);
  } > rtc_slow_seg

  /* Get size of rtc slow data based on rtc_data_location alias */
  _rtc_slow_length = (ORIGIN(rtc_slow_seg) == ORIGIN(rtc_data_location))
                        ? (_rtc_force_slow_end - _rtc_data_start)
                        : (_rtc_force_slow_end - _rtc_force_slow_start);

  _rtc_fast_length = (ORIGIN(rtc_slow_seg) == ORIGIN(rtc_data_location))
                        ? (_rtc_force_fast_end - _rtc_fast_start)
                        : (_rtc_noinit_end - _rtc_fast_start);

  ASSERT((_rtc_slow_length <= LENGTH(rtc_slow_seg)),
          "RTC_SLOW segment data does not fit.")

  ASSERT((_rtc_fast_length <= LENGTH(rtc_data_seg)),
          "RTC_FAST segment data does not fit.")

  /* Send .iram0 code to iram */
  .iram0.vectors :
  {
    _iram_start = ABSOLUTE(.);
    /* Vectors go to IRAM */
    _init_start = ABSOLUTE(.);
    /* Vectors according to builds/RF-2015.2-win32/esp108_v1_2_s5_512int_2/config.html */
    . = 0x0;
    KEEP(*(.WindowVectors.text));
    . = 0x180;
    KEEP(*(.Level2InterruptVector.text));
    . = 0x1c0;
    KEEP(*(.Level3InterruptVector.text));
    . = 0x200;
    KEEP(*(.Level4InterruptVector.text));
    . = 0x240;
    KEEP(*(.Level5InterruptVector.text));
    . = 0x280;
    KEEP(*(.DebugExceptionVector.text));
    . = 0x2c0;
    KEEP(*(.NMIExceptionVector.text));
    . = 0x300;
    KEEP(*(.KernelExceptionVector.text));
    . = 0x340;
    KEEP(*(.UserExceptionVector.text));
    . = 0x3C0;
    KEEP(*(.DoubleExceptionVector.text));
    . = 0x400;
    _invalid_pc_placeholder = ABSOLUTE(.);
    *(.*Vector.literal)

    *(.UserEnter.literal);
    *(.UserEnter.text);
    . = ALIGN (16);
    *(.entry.text)
    *(.init.literal)
    *(.init)
    _init_end = ABSOLUTE(.);
  } > iram0_0_seg

  .iram0.text :
  {
    /* Code marked as runnning out of IRAM */
    _iram_text_start = ABSOLUTE(.);

    *(EXCLUDE_FILE(*libsoc.a:uart_hal_iram.* *libsoc.a:twai_hal_iram.*) .iram1 EXCLUDE_FILE(*libsoc.a:uart_hal_iram.* *libsoc.a:twai_hal_iram.*) .iram1.*)
    *libapp_trace.a:app_trace.*( .literal  .literal.*  .text  .text.*)
    *libapp_trace.a:app_trace_util.*( .literal  .literal.*  .text  .text.*)
    *libdriver.a:gpio.*(.literal.gpio_iomux_in .text.gpio_iomux_in)
    *libdriver.a:gpio.*(.literal.gpio_iomux_out .text.gpio_iomux_out)
    *libesp_common.a:esp_err.*( .literal  .literal.*  .text  .text.*)
    *libesp_event.a:default_event_loop.*(.literal.esp_event_isr_post .text.esp_event_isr_post)
    *libesp_event.a:esp_event.*(.literal.esp_event_isr_post_to .text.esp_event_isr_post_to)
    *libesp_ringbuf.a:( .literal  .literal.*  .text  .text.*)
    *libesp_system.a:panic.*( .literal  .literal.*  .text  .text.*)
    *libesp_system.a:panic_handler.*( .literal  .literal.*  .text  .text.*)
    *libesp_system.a:system_api.*(.literal.esp_system_abort .text.esp_system_abort)
    *libfreertos.a:( .literal  .literal.*  .text  .text.*)
    *libgcc.a:_divsf3.*( .literal  .literal.*  .text  .text.*)
    *libgcc.a:lib2funcs.*( .literal  .literal.*  .text  .text.*)
    *libgcov.a:( .literal  .literal.*  .text  .text.*)
    *libhal.a:( .literal  .literal.*  .text  .text.*)
    *libheap.a:multi_heap.*( .literal  .literal.*  .text  .text.*)
    *liblog.a:log.*(.literal.esp_log_write .text.esp_log_write)
    *liblog.a:log_freertos.*(.literal.esp_log_early_timestamp .text.esp_log_early_timestamp)
    *liblog.a:log_freertos.*(.literal.esp_log_impl_lock .text.esp_log_impl_lock)
    *liblog.a:log_freertos.*(.literal.esp_log_impl_lock_timeout .text.esp_log_impl_lock_timeout)
    *liblog.a:log_freertos.*(.literal.esp_log_impl_unlock .text.esp_log_impl_unlock)
    *liblog.a:log_freertos.*(.literal.esp_log_timestamp .text.esp_log_timestamp)
    *libnet80211.a:( .wifi0iram  .wifi0iram.*)
    *libnet80211.a:( .wifirxiram  .wifirxiram.*)
    *libnewlib.a:abort.*( .literal  .literal.*  .text  .text.*)
    *libnewlib.a:heap.*( .literal  .literal.*  .text  .text.*)
    *libpp.a:( .wifi0iram  .wifi0iram.*)
    *libpp.a:( .wifirxiram  .wifirxiram.*)
    *librtc.a:( .literal  .literal.*  .text  .text.*)
    *libsoc.a:cpu_hal.*( .literal  .literal.*  .text  .text.*)
    *libsoc.a:cpu_util.*( .literal  .literal.*  .text  .text.*)
    *libsoc.a:i2c_hal_iram.*( .literal  .literal.*  .text  .text.*)
    *libsoc.a:ledc_hal_iram.*( .literal  .literal.*  .text  .text.*)
    *libsoc.a:lldesc.*( .literal  .literal.*  .text  .text.*)
    *libsoc.a:rtc_clk.*( .literal  .literal.*  .text  .text.*)
    *libsoc.a:rtc_clk_init.*( .literal  .literal.*  .text  .text.*)
    *libsoc.a:rtc_init.*( .literal  .literal.*  .text  .text.*)
    *libsoc.a:rtc_pm.*( .literal  .literal.*  .text  .text.*)
    *libsoc.a:rtc_sleep.*( .literal  .literal.*  .text  .text.*)
    *libsoc.a:rtc_time.*( .literal  .literal.*  .text  .text.*)
    *libsoc.a:rtc_wdt.*( .literal  .literal.*  .text  .text.*)
    *libsoc.a:soc_hal.*( .literal  .literal.*  .text  .text.*)
    *libsoc.a:spi_flash_hal_iram.*( .literal  .literal.*  .text  .text.*)
    *libsoc.a:spi_hal_iram.*( .literal  .literal.*  .text  .text.*)
    *libsoc.a:spi_slave_hal_iram.*( .literal  .literal.*  .text  .text.*)
    *libsoc.a:twai_hal_iram.*( .iram1  .iram1.*)
    *libsoc.a:uart_hal_iram.*( .iram1  .iram1.*)
    *libsoc.a:wdt_hal_iram.*( .literal  .literal.*  .text  .text.*)
    *libspi_flash.a:memspi_host_driver.*( .literal  .literal.*  .text  .text.*)
    *libspi_flash.a:spi_flash_chip_gd.*( .literal  .literal.*  .text  .text.*)
    *libspi_flash.a:spi_flash_chip_generic.*( .literal  .literal.*  .text  .text.*)
    *libspi_flash.a:spi_flash_chip_issi.*( .literal  .literal.*  .text  .text.*)
    *libspi_flash.a:spi_flash_chip_mxic.*( .literal  .literal.*  .text  .text.*)
    *libspi_flash.a:spi_flash_rom_patch.*( .literal  .literal.*  .text  .text.*)
    *libxtensa.a:eri.*( .literal  .literal.*  .text  .text.*)

  } > iram0_0_seg

  .dram0.data :
  {
    _data_start = ABSOLUTE(.);
    _bt_data_start = ABSOLUTE(.);
    *libbt.a:(.data .data.*)
    . = ALIGN (4);
    _bt_data_end = ABSOLUTE(.);
    _btdm_data_start = ABSOLUTE(.);
    *libbtdm_app.a:(.data .data.*)
    . = ALIGN (4);
    _btdm_data_end = ABSOLUTE(.);
    _nimble_data_start = ABSOLUTE(.);
    *libnimble.a:(.data .data.*)
    . = ALIGN (4);
    _nimble_data_end = ABSOLUTE(.);
    *(.gnu.linkonce.d.*)
    *(.data1)
    *(.sdata)
    *(.sdata.*)
    *(.gnu.linkonce.s.*)
    *(.sdata2)
    *(.sdata2.*)
    *(.gnu.linkonce.s2.*)
    *(.jcr)


    *(EXCLUDE_FILE(*libsoc.a:uart_hal_iram.* *libsoc.a:twai_hal_iram.*) .data EXCLUDE_FILE(*libsoc.a:uart_hal_iram.* *libsoc.a:twai_hal_iram.*) .data.* EXCLUDE_FILE(*libsoc.a:uart_hal_iram.* *libsoc.a:twai_hal_iram.*) .dram1 EXCLUDE_FILE(*libsoc.a:uart_hal_iram.* *libsoc.a:twai_hal_iram.*) .dram1.*)
    *libapp_trace.a:app_trace.*( .rodata  .rodata.*)
    *libapp_trace.a:app_trace_util.*( .rodata  .rodata.*)
    *libdriver.a:gpio.*(.rodata.gpio_iomux_in)
    *libdriver.a:gpio.*(.rodata.gpio_iomux_out)
    *libesp_common.a:esp_err.*( .rodata  .rodata.*)
    *libesp_event.a:default_event_loop.*(.rodata.esp_event_isr_post)
    *libesp_event.a:esp_event.*(.rodata.esp_event_isr_post_to)
    *libesp_system.a:panic.*( .rodata  .rodata.*)
    *libesp_system.a:panic_handler.*( .rodata  .rodata.*)
    *libesp_system.a:system_api.*(.rodata.esp_system_abort)
    *libgcc.a:_divsf3.*( .rodata  .rodata.*)
    *libgcov.a:( .rodata  .rodata.*)
    *libheap.a:multi_heap.*( .rodata  .rodata.*)
    *liblog.a:log.*(.rodata.esp_log_write)
    *liblog.a:log_freertos.*(.rodata.esp_log_early_timestamp)
    *liblog.a:log_freertos.*(.rodata.esp_log_impl_lock)
    *liblog.a:log_freertos.*(.rodata.esp_log_impl_lock_timeout)
    *liblog.a:log_freertos.*(.rodata.esp_log_impl_unlock)
    *liblog.a:log_freertos.*(.rodata.esp_log_timestamp)
    *libnewlib.a:abort.*( .rodata  .rodata.*)
    *libnewlib.a:heap.*( .rodata  .rodata.*)
    *libphy.a:( .rodata  .rodata.*)
    *libsoc.a:cpu_hal.*( .rodata  .rodata.*)
    *libsoc.a:i2c_hal_iram.*( .rodata  .rodata.*)
    *libsoc.a:ledc_hal_iram.*( .rodata  .rodata.*)
    *libsoc.a:lldesc.*( .rodata  .rodata.*)
    *libsoc.a:rtc_clk.*( .rodata  .rodata.*)
    *libsoc.a:soc_hal.*( .rodata  .rodata.*)
    *libsoc.a:spi_flash_hal_iram.*( .rodata  .rodata.*)
    *libsoc.a:spi_hal_iram.*( .rodata  .rodata.*)
    *libsoc.a:spi_slave_hal_iram.*( .rodata  .rodata.*)
    *libsoc.a:twai_hal_iram.*( .data  .data.*  .dram1  .dram1.*)
    *libsoc.a:uart_hal_iram.*( .data  .data.*  .dram1  .dram1.*)
    *libsoc.a:wdt_hal_iram.*( .rodata  .rodata.*)
    *libspi_flash.a:memspi_host_driver.*( .rodata  .rodata.*)
    *libspi_flash.a:spi_flash_chip_gd.*( .rodata  .rodata.*)
    *libspi_flash.a:spi_flash_chip_generic.*( .rodata  .rodata.*)
    *libspi_flash.a:spi_flash_chip_issi.*( .rodata  .rodata.*)
    *libspi_flash.a:spi_flash_chip_mxic.*( .rodata  .rodata.*)
    *libspi_flash.a:spi_flash_rom_patch.*( .rodata  .rodata.*)

    _data_end = ABSOLUTE(.);
    . = ALIGN(4);
  } > dram0_0_seg

  /*This section holds data that should not be initialized at power up.
    The section located in Internal SRAM memory region. The macro _NOINIT
    can be used as attribute to place data into this section.
    See the esp_attr.h file for more information.
  */
  .noinit (NOLOAD):
  {
    . = ALIGN(4);
    _noinit_start = ABSOLUTE(.);
    *(.noinit .noinit.*)
    . = ALIGN(4) ;
    _noinit_end = ABSOLUTE(.);
  } > dram0_0_seg

  /* Shared RAM */
  .dram0.bss (NOLOAD) :
  {
    . = ALIGN (8);
    _bss_start = ABSOLUTE(.);
    *(.ext_ram.bss*)
    _bt_bss_start = ABSOLUTE(.);
    *libbt.a:(.bss .bss.* COMMON)
    . = ALIGN (4);
    _bt_bss_end = ABSOLUTE(.);
    _btdm_bss_start = ABSOLUTE(.);
    *libbtdm_app.a:(.bss .bss.* COMMON)
    . = ALIGN (4);
    _btdm_bss_end = ABSOLUTE(.);
    _nimble_bss_start = ABSOLUTE(.);
    *libnimble.a:(.bss .bss.* COMMON)
    . = ALIGN (4);
    _nimble_bss_end = ABSOLUTE(.);

    *(EXCLUDE_FILE(*libsoc.a:uart_hal_iram.* *libsoc.a:twai_hal_iram.*) .bss EXCLUDE_FILE(*libsoc.a:uart_hal_iram.* *libsoc.a:twai_hal_iram.*) .bss.* EXCLUDE_FILE(*libsoc.a:uart_hal_iram.* *libsoc.a:twai_hal_iram.*) COMMON)
    *libsoc.a:twai_hal_iram.*( .bss  .bss.*  COMMON)
    *libsoc.a:uart_hal_iram.*( .bss  .bss.*  COMMON)

    *(.dynsbss)
    *(.sbss)
    *(.sbss.*)
    *(.gnu.linkonce.sb.*)
    *(.scommon)
    *(.sbss2)
    *(.sbss2.*)
    *(.gnu.linkonce.sb2.*)
    *(.dynbss)
    *(.share.mem)
    *(.gnu.linkonce.b.*)

    . = ALIGN (8);
    _bss_end = ABSOLUTE(.);
  } > dram0_0_seg

  ASSERT(((_bss_end - ORIGIN(dram0_0_seg)) <= LENGTH(dram0_0_seg)),
          "DRAM segment data does not fit.")

  .flash.appdesc : ALIGN(0x10)
  {
    _rodata_start = ABSOLUTE(.);

    *(.rodata_desc .rodata_desc.*)               /* Should be the first.  App version info.        DO NOT PUT ANYTHING BEFORE IT! */
    *(.rodata_custom_desc .rodata_custom_desc.*) /* Should be the second. Custom app version info. DO NOT PUT ANYTHING BEFORE IT! */

    /* Create an empty gap within this section. Thanks to this, the end of this
     * section will match .flah.rodata's begin address. Thus, both sections
     * will be merged when creating the final bin image. */
    . = ALIGN(ALIGNOF(.flash.rodata));
  } >default_rodata_seg

  .flash.rodata : ALIGN(0x10)
  {
    _flash_rodata_start = ABSOLUTE(.);

    *(EXCLUDE_FILE(*libspi_flash.a:spi_flash_chip_gd.* *libspi_flash.a:spi_flash_chip_generic.* *libspi_flash.a:memspi_host_driver.* *libspi_flash.a:spi_flash_chip_mxic.* *libspi_flash.a:spi_flash_chip_issi.* *libspi_flash.a:spi_flash_rom_patch.* *libesp_system.a:panic.* *libesp_system.a:panic_handler.* *libsoc.a:uart_hal_iram.* *libsoc.a:rtc_clk.* *libsoc.a:twai_hal_iram.* *libsoc.a:soc_hal.* *libsoc.a:spi_hal_iram.* *libsoc.a:lldesc.* *libsoc.a:ledc_hal_iram.* *libsoc.a:spi_flash_hal_iram.* *libsoc.a:spi_slave_hal_iram.* *libsoc.a:i2c_hal_iram.* *libsoc.a:cpu_hal.* *libsoc.a:wdt_hal_iram.* *libphy.a *libheap.a:multi_heap.* *libgcc.a:_divsf3.* *libgcov.a *libesp_common.a:esp_err.* *libnewlib.a:abort.* *libnewlib.a:heap.* *libapp_trace.a:app_trace_util.* *libapp_trace.a:app_trace.*) .rodata EXCLUDE_FILE(*libspi_flash.a:spi_flash_chip_gd.* *libspi_flash.a:spi_flash_chip_generic.* *libspi_flash.a:memspi_host_driver.* *libspi_flash.a:spi_flash_chip_mxic.* *libspi_flash.a:spi_flash_chip_issi.* *libspi_flash.a:spi_flash_rom_patch.* *libdriver.a:gpio.* *libesp_system.a:system_api.* *libesp_system.a:panic.* *libesp_system.a:panic_handler.* *libsoc.a:uart_hal_iram.* *libsoc.a:rtc_clk.* *libsoc.a:twai_hal_iram.* *libsoc.a:soc_hal.* *libsoc.a:spi_hal_iram.* *libsoc.a:lldesc.* *libsoc.a:ledc_hal_iram.* *libsoc.a:spi_flash_hal_iram.* *libsoc.a:spi_slave_hal_iram.* *libsoc.a:i2c_hal_iram.* *libsoc.a:cpu_hal.* *libsoc.a:wdt_hal_iram.* *libesp_event.a:default_event_loop.* *libesp_event.a:esp_event.* *libphy.a *liblog.a:log.* *liblog.a:log_freertos.* *libheap.a:multi_heap.* *libgcc.a:_divsf3.* *libgcov.a *libesp_common.a:esp_err.* *libnewlib.a:abort.* *libnewlib.a:heap.* *libapp_trace.a:app_trace_util.* *libapp_trace.a:app_trace.*) .rodata.*)
    *libdriver.a:gpio.*(.rodata.gpio_od_enable.str1.4 .rodata.gpio_input_enable.str1.4 .rodata.gpio_input_disable.str1.4 .rodata.gpio_output_disable.str1.4 .rodata.gpio_output_enable.str1.4 .rodata.gpio_pullup_en.str1.4 .rodata.gpio_pullup_dis.str1.4 .rodata.gpio_set_intr_type.str1.4 .rodata.gpio_set_pull_mode.str1.4 .rodata.gpio_set_direction.str1.4 .rodata.gpio_config.str1.4 .rodata.gpio_reset_pin.str1.4 .rodata.gpio_isr_handler_add.str1.4 .rodata.gpio_isr_register.str1.4 .rodata.gpio_install_isr_service.str1.4 .rodata.gpio_wakeup_enable.str1.4 .rodata.gpio_set_drive_capability.str1.4 .rodata.gpio_get_drive_capability.str1.4 .rodata.gpio_hold_en.str1.4 .rodata.gpio_iomux_in.str1.4 .rodata.gpio_iomux_out.str1.4 .rodata.__func__$6519 .rodata.__func__$6509 .rodata.__FUNCTION__$6742 .rodata.__FUNCTION__$6737 .rodata.__func__$6471 .rodata.__FUNCTION__$6732 .rodata.__func__$6462 .rodata.__FUNCTION__$6726 .rodata.__FUNCTION__$6720 .rodata.__FUNCTION__$6715 .rodata.__FUNCTION__$6708 .rodata.__FUNCTION__$6691 .rodata.__FUNCTION__$6687 .rodata.__FUNCTION__$6680 .rodata.__func__$6661 .rodata.__func__$6652 .rodata.__FUNCTION__$6610 .rodata.__FUNCTION__$6614 .rodata.__func__$6416 .rodata.__FUNCTION__$6602 .rodata.__FUNCTION__$6606 .rodata.__func__$6398 .rodata.__FUNCTION__$6594 .rodata.__func__$6406 .rodata.__FUNCTION__$6598 .rodata.__FUNCTION__$6639 .rodata.__FUNCTION__$6627 .rodata.__FUNCTION__$6619 .rodata.__FUNCTION__$6590 .rodata.__FUNCTION__$6582 .rodata.__FUNCTION__$6586 .rodata.__FUNCTION__$6577 .rodata.__func__$6352 .rodata.__FUNCTION__$6572 .rodata.__func__$6346 .rodata.__FUNCTION__$6568 .rodata.__func__$6340 .rodata.__FUNCTION__$6564 .rodata.__func__$6334 .rodata.__FUNCTION__$6560)
    *libesp_event.a:default_event_loop.*(.rodata.esp_event_loop_create_default.str1.4 .rodata.esp_event_send_to_default_loop)
    *libesp_event.a:esp_event.*(.rodata.handler_instances_add.str1.4 .rodata.base_node_add_handler.str1.4 .rodata.loop_node_add_handler.str1.4 .rodata.esp_event_loop_create.str1.4 .rodata.esp_event_loop_run.str1.4 .rodata.esp_event_loop_run_task.str1.4 .rodata.esp_event_handler_register_with_internal.str1.4 .rodata.esp_event_handler_unregister_with_internal.str1.4 .rodata.__func__$8943 .rodata.__func__$8930 .rodata.__func__$8897 .rodata.__func__$8865 .rodata.__func__$8840 .rodata.__func__$8799)
    *libesp_system.a:system_api.*(.rodata.esp_get_idf_version.str1.4)
    *liblog.a:log.*(.rodata.esp_log_level_set.str1.4 .rodata.__func__$3534 .rodata.__func__$3505)
    *liblog.a:log_freertos.*(.rodata.esp_log_system_timestamp.str1.4)
    *libsoc.a:twai_hal_iram.*( .rodata  .rodata.*)
    *libsoc.a:uart_hal_iram.*( .rodata  .rodata.*)

    *(.irom1.text) /* catch stray ICACHE_RODATA_ATTR */
    *(.gnu.linkonce.r.*)
    *(.rodata1)
    __XT_EXCEPTION_TABLE_ = ABSOLUTE(.);
    *(.xt_except_table)
    *(.gcc_except_table .gcc_except_table.*)
    *(.gnu.linkonce.e.*)
    *(.gnu.version_r)
    . = (. + 3) & ~ 3;
    __eh_frame = ABSOLUTE(.);
    KEEP(*(.eh_frame))
    . = (. + 7) & ~ 3;
    /*  C++ constructor and destructor tables

        Make a point of not including anything from crtbegin.o or crtend.o, as IDF doesn't use toolchain crt
      */
    __init_array_start = ABSOLUTE(.);
    KEEP (*(EXCLUDE_FILE (*crtend.* *crtbegin.*) .ctors SORT(.ctors.*)))
    __init_array_end = ABSOLUTE(.);
    KEEP (*crtbegin.*(.dtors))
    KEEP (*(EXCLUDE_FILE (*crtend.*) .dtors))
    KEEP (*(SORT(.dtors.*)))
    KEEP (*(.dtors))
    /*  C++ exception handlers table:  */
    __XT_EXCEPTION_DESCS_ = ABSOLUTE(.);
    *(.xt_except_desc)
    *(.gnu.linkonce.h.*)
    __XT_EXCEPTION_DESCS_END__ = ABSOLUTE(.);
    *(.xt_except_desc_end)
    *(.dynamic)
    *(.gnu.version_d)
    /* Addresses of memory regions reserved via
       SOC_RESERVE_MEMORY_REGION() */
    soc_reserved_memory_region_start = ABSOLUTE(.);
    KEEP (*(.reserved_memory_address))
    soc_reserved_memory_region_end = ABSOLUTE(.);
    _rodata_end = ABSOLUTE(.);
    /* Literals are also RO data. */
    _lit4_start = ABSOLUTE(.);
    *(*.lit4)
    *(.lit4.*)
    *(.gnu.linkonce.lit4.*)
    _lit4_end = ABSOLUTE(.);
    . = ALIGN(4);
    _thread_local_start = ABSOLUTE(.);
    *(.tdata)
    *(.tdata.*)
    *(.tbss)
    *(.tbss.*)
    _thread_local_end = ABSOLUTE(.);
    . = ALIGN(4);
  } >default_rodata_seg

  _flash_rodata_align = ALIGNOF(.flash.rodata);

  .flash.text :
  {
    _stext = .;
    _text_start = ABSOLUTE(.);

    *(EXCLUDE_FILE(*libxtensa.a:eri.* *libhal.a *libspi_flash.a:spi_flash_chip_gd.* *libspi_flash.a:spi_flash_chip_generic.* *libspi_flash.a:memspi_host_driver.* *libspi_flash.a:spi_flash_chip_mxic.* *libspi_flash.a:spi_flash_chip_issi.* *libspi_flash.a:spi_flash_rom_patch.* *libesp_system.a:panic.* *libesp_system.a:panic_handler.* *libsoc.a:uart_hal_iram.* *libsoc.a:rtc_clk.* *libsoc.a:twai_hal_iram.* *libsoc.a:soc_hal.* *libsoc.a:rtc_sleep.* *libsoc.a:spi_hal_iram.* *libsoc.a:rtc_clk_init.* *libsoc.a:rtc_pm.* *libsoc.a:rtc_wdt.* *libsoc.a:lldesc.* *libsoc.a:rtc_time.* *libsoc.a:ledc_hal_iram.* *libsoc.a:rtc_init.* *libsoc.a:spi_flash_hal_iram.* *libsoc.a:cpu_util.* *libsoc.a:spi_slave_hal_iram.* *libsoc.a:i2c_hal_iram.* *libsoc.a:cpu_hal.* *libsoc.a:wdt_hal_iram.* *librtc.a *libheap.a:multi_heap.* *libesp_ringbuf.a *libgcc.a:lib2funcs.* *libgcc.a:_divsf3.* *libgcov.a *libesp_common.a:esp_err.* *libfreertos.a *libnewlib.a:abort.* *libnewlib.a:heap.* *libapp_trace.a:app_trace_util.* *libapp_trace.a:app_trace.*) .literal EXCLUDE_FILE(*libxtensa.a:eri.* *libhal.a *libspi_flash.a:spi_flash_chip_gd.* *libspi_flash.a:spi_flash_chip_generic.* *libspi_flash.a:memspi_host_driver.* *libspi_flash.a:spi_flash_chip_mxic.* *libspi_flash.a:spi_flash_chip_issi.* *libspi_flash.a:spi_flash_rom_patch.* *libdriver.a:gpio.* *libesp_system.a:system_api.* *libesp_system.a:panic.* *libesp_system.a:panic_handler.* *libsoc.a:uart_hal_iram.* *libsoc.a:rtc_clk.* *libsoc.a:twai_hal_iram.* *libsoc.a:soc_hal.* *libsoc.a:rtc_sleep.* *libsoc.a:spi_hal_iram.* *libsoc.a:rtc_clk_init.* *libsoc.a:rtc_pm.* *libsoc.a:rtc_wdt.* *libsoc.a:lldesc.* *libsoc.a:rtc_time.* *libsoc.a:ledc_hal_iram.* *libsoc.a:rtc_init.* *libsoc.a:spi_flash_hal_iram.* *libsoc.a:cpu_util.* *libsoc.a:spi_slave_hal_iram.* *libsoc.a:i2c_hal_iram.* *libsoc.a:cpu_hal.* *libsoc.a:wdt_hal_iram.* *libesp_event.a:default_event_loop.* *libesp_event.a:esp_event.* *librtc.a *liblog.a:log.* *liblog.a:log_freertos.* *libheap.a:multi_heap.* *libesp_ringbuf.a *libgcc.a:lib2funcs.* *libgcc.a:_divsf3.* *libgcov.a *libesp_common.a:esp_err.* *libfreertos.a *libnewlib.a:abort.* *libnewlib.a:heap.* *libapp_trace.a:app_trace_util.* *libapp_trace.a:app_trace.*) .literal.* EXCLUDE_FILE(*libsoc.a:uart_hal_iram.* *libsoc.a:twai_hal_iram.*) .phyiram EXCLUDE_FILE(*libsoc.a:uart_hal_iram.* *libsoc.a:twai_hal_iram.*) .phyiram.* EXCLUDE_FILE(*libxtensa.a:eri.* *libhal.a *libspi_flash.a:spi_flash_chip_gd.* *libspi_flash.a:spi_flash_chip_generic.* *libspi_flash.a:memspi_host_driver.* *libspi_flash.a:spi_flash_chip_mxic.* *libspi_flash.a:spi_flash_chip_issi.* *libspi_flash.a:spi_flash_rom_patch.* *libesp_system.a:panic.* *libesp_system.a:panic_handler.* *libsoc.a:uart_hal_iram.* *libsoc.a:rtc_clk.* *libsoc.a:twai_hal_iram.* *libsoc.a:soc_hal.* *libsoc.a:rtc_sleep.* *libsoc.a:spi_hal_iram.* *libsoc.a:rtc_clk_init.* *libsoc.a:rtc_pm.* *libsoc.a:rtc_wdt.* *libsoc.a:lldesc.* *libsoc.a:rtc_time.* *libsoc.a:ledc_hal_iram.* *libsoc.a:rtc_init.* *libsoc.a:spi_flash_hal_iram.* *libsoc.a:cpu_util.* *libsoc.a:spi_slave_hal_iram.* *libsoc.a:i2c_hal_iram.* *libsoc.a:cpu_hal.* *libsoc.a:wdt_hal_iram.* *librtc.a *libheap.a:multi_heap.* *libesp_ringbuf.a *libgcc.a:lib2funcs.* *libgcc.a:_divsf3.* *libgcov.a *libesp_common.a:esp_err.* *libfreertos.a *libnewlib.a:abort.* *libnewlib.a:heap.* *libapp_trace.a:app_trace_util.* *libapp_trace.a:app_trace.*) .text EXCLUDE_FILE(*libxtensa.a:eri.* *libhal.a *libspi_flash.a:spi_flash_chip_gd.* *libspi_flash.a:spi_flash_chip_generic.* *libspi_flash.a:memspi_host_driver.* *libspi_flash.a:spi_flash_chip_mxic.* *libspi_flash.a:spi_flash_chip_issi.* *libspi_flash.a:spi_flash_rom_patch.* *libdriver.a:gpio.* *libesp_system.a:system_api.* *libesp_system.a:panic.* *libesp_system.a:panic_handler.* *libsoc.a:uart_hal_iram.* *libsoc.a:rtc_clk.* *libsoc.a:twai_hal_iram.* *libsoc.a:soc_hal.* *libsoc.a:rtc_sleep.* *libsoc.a:spi_hal_iram.* *libsoc.a:rtc_clk_init.* *libsoc.a:rtc_pm.* *libsoc.a:rtc_wdt.* *libsoc.a:lldesc.* *libsoc.a:rtc_time.* *libsoc.a:ledc_hal_iram.* *libsoc.a:rtc_init.* *libsoc.a:spi_flash_hal_iram.* *libsoc.a:cpu_util.* *libsoc.a:spi_slave_hal_iram.* *libsoc.a:i2c_hal_iram.* *libsoc.a:cpu_hal.* *libsoc.a:wdt_hal_iram.* *libesp_event.a:default_event_loop.* *libesp_event.a:esp_event.* *librtc.a *liblog.a:log.* *liblog.a:log_freertos.* *libheap.a:multi_heap.* *libesp_ringbuf.a *libgcc.a:lib2funcs.* *libgcc.a:_divsf3.* *libgcov.a *libesp_common.a:esp_err.* *libfreertos.a *libnewlib.a:abort.* *libnewlib.a:heap.* *libapp_trace.a:app_trace_util.* *libapp_trace.a:app_trace.*) .text.* EXCLUDE_FILE(*libsoc.a:uart_hal_iram.* *libsoc.a:twai_hal_iram.* *libpp.a *libnet80211.a) .wifi0iram EXCLUDE_FILE(*libsoc.a:uart_hal_iram.* *libsoc.a:twai_hal_iram.* *libpp.a *libnet80211.a) .wifi0iram.* EXCLUDE_FILE(*libsoc.a:uart_hal_iram.* *libsoc.a:twai_hal_iram.* *libpp.a *libnet80211.a) .wifirxiram EXCLUDE_FILE(*libsoc.a:uart_hal_iram.* *libsoc.a:twai_hal_iram.* *libpp.a *libnet80211.a) .wifirxiram.*)
    *libdriver.a:gpio.*(.literal.gpio_od_enable .literal.gpio_od_disable .literal.gpio_input_enable .literal.gpio_input_disable .literal.gpio_output_disable .literal.gpio_intr_enable_on_core .literal.gpio_output_enable .literal.gpio_isr_register_on_core_static .literal.gpio_pullup_en .literal.gpio_pullup_dis .literal.gpio_pulldown_en .literal.gpio_pulldown_dis .literal.gpio_set_intr_type .literal.gpio_intr_enable .literal.gpio_intr_disable .literal.gpio_set_level .literal.gpio_get_level .literal.gpio_set_pull_mode .literal.gpio_set_direction .literal.gpio_config .literal.gpio_reset_pin .literal.gpio_isr_handler_add .literal.gpio_isr_handler_remove .literal.gpio_uninstall_isr_service .literal.gpio_isr_register .literal.gpio_install_isr_service .literal.gpio_wakeup_enable .literal.gpio_wakeup_disable .literal.gpio_set_drive_capability .literal.gpio_get_drive_capability .literal.gpio_hold_en .literal.gpio_hold_dis .literal.gpio_deep_sleep_hold_en .literal.gpio_deep_sleep_hold_dis .text.gpio_od_enable .text.gpio_od_disable .text.gpio_input_enable .text.gpio_input_disable .text.gpio_output_disable .text.gpio_intr_enable_on_core .text.gpio_output_enable .text.gpio_isr_register_on_core_static .text.gpio_pullup_en .text.gpio_pullup_dis .text.gpio_pulldown_en .text.gpio_pulldown_dis .text.gpio_set_intr_type .text.gpio_intr_enable .text.gpio_intr_disable .text.gpio_set_level .text.gpio_get_level .text.gpio_set_pull_mode .text.gpio_set_direction .text.gpio_config .text.gpio_reset_pin .text.gpio_isr_handler_add .text.gpio_isr_handler_remove .text.gpio_uninstall_isr_service .text.gpio_isr_register .text.gpio_install_isr_service .text.gpio_wakeup_enable .text.gpio_wakeup_disable .text.gpio_set_drive_capability .text.gpio_get_drive_capability .text.gpio_hold_en .text.gpio_hold_dis .text.gpio_deep_sleep_hold_en .text.gpio_deep_sleep_hold_dis)
    *libesp_event.a:default_event_loop.*(.literal.esp_event_handler_register .literal.esp_event_handler_instance_register .literal.esp_event_handler_unregister .literal.esp_event_handler_instance_unregister .literal.esp_event_post .literal.esp_event_loop_create_default .literal.esp_event_loop_delete_default .literal.esp_event_send_to_default_loop .text.esp_event_handler_register .text.esp_event_handler_instance_register .text.esp_event_handler_unregister .text.esp_event_handler_instance_unregister .text.esp_event_post .text.esp_event_loop_create_default .text.esp_event_loop_delete_default .text.esp_event_send_to_default_loop)
    *libesp_event.a:esp_event.*(.literal.handler_instances_remove_all .literal.base_node_remove_all_handler .literal.loop_node_remove_all_handler .literal.handler_instances_add .literal.base_node_add_handler .literal.loop_node_add_handler .literal.handler_instances_remove .literal.base_node_remove_handler .literal.loop_node_remove_handler .literal.esp_event_loop_create .literal.esp_event_loop_run .literal.esp_event_loop_run_task .literal.esp_event_loop_delete .literal.esp_event_handler_register_with_internal .literal.esp_event_handler_register_with .literal.esp_event_handler_instance_register_with .literal.esp_event_handler_unregister_with_internal .literal.esp_event_handler_unregister_with .literal.esp_event_handler_instance_unregister_with .literal.esp_event_post_to .text.handler_execute .text.handler_instances_remove_all .text.base_node_remove_all_handler .text.loop_node_remove_all_handler .text.handler_instances_add .text.base_node_add_handler .text.loop_node_add_handler .text.handler_instances_remove .text.base_node_remove_handler .text.loop_node_remove_handler .text.esp_event_loop_create .text.esp_event_loop_run .text.esp_event_loop_run_task .text.esp_event_loop_delete .text.esp_event_handler_register_with_internal .text.esp_event_handler_register_with .text.esp_event_handler_instance_register_with .text.esp_event_handler_unregister_with_internal .text.esp_event_handler_unregister_with .text.esp_event_handler_instance_unregister_with .text.esp_event_post_to .text.esp_event_dump)
    *libesp_system.a:system_api.*(.literal.esp_register_shutdown_handler .literal.esp_unregister_shutdown_handler .literal.esp_get_free_heap_size .literal.esp_get_free_internal_heap_size .literal.esp_get_minimum_free_heap_size .literal.esp_get_idf_version .text.esp_register_shutdown_handler .text.esp_unregister_shutdown_handler .text.esp_get_free_heap_size .text.esp_get_free_internal_heap_size .text.esp_get_minimum_free_heap_size .text.esp_get_idf_version)
    *liblog.a:log.*(.literal.heap_bubble_down .literal.esp_log_set_vprintf .literal.esp_log_level_set .literal.esp_log_writev .text.heap_bubble_down .text.esp_log_set_vprintf .text.esp_log_level_set .text.esp_log_writev)
    *liblog.a:log_freertos.*(.literal.esp_log_system_timestamp .text.esp_log_system_timestamp)
    *libsoc.a:twai_hal_iram.*( .literal  .literal.*  .phyiram  .phyiram.*  .text  .text.*  .wifi0iram  .wifi0iram.*  .wifirxiram  .wifirxiram.*)
    *libsoc.a:uart_hal_iram.*( .literal  .literal.*  .phyiram  .phyiram.*  .text  .text.*  .wifi0iram  .wifi0iram.*  .wifirxiram  .wifirxiram.*)

    *(.stub .gnu.warning .gnu.linkonce.literal.* .gnu.linkonce.t.*.literal .gnu.linkonce.t.*)
    *(.irom0.text) /* catch stray ICACHE_RODATA_ATTR */
    *(.fini.literal)
    *(.fini)
    *(.gnu.version)

    /** CPU will try to prefetch up to 16 bytes of
      * of instructions. This means that any configuration (e.g. MMU, PMS) must allow
      * safe access to up to 16 bytes after the last real instruction, add
      * dummy bytes to ensure this
      */
    . += 16;

    _text_end = ABSOLUTE(.);
    _etext = .;

    /* Similar to _iram_start, this symbol goes here so it is
       resolved by addr2line in preference to the first symbol in
       the flash.text segment.
    */
    _flash_cache_start = ABSOLUTE(0);
  } >default_code_seg

  /* Marks the end of IRAM code segment */
  .iram0.text_end (NOLOAD) :
  {
    . = ALIGN (4);
    _iram_text_end = ABSOLUTE(.);
  } > iram0_0_seg

  .iram0.data :
  {
    . = ALIGN(4);
    _iram_data_start = ABSOLUTE(.);

    *(EXCLUDE_FILE(*libsoc.a:uart_hal_iram.* *libsoc.a:twai_hal_iram.*) .iram.data EXCLUDE_FILE(*libsoc.a:uart_hal_iram.* *libsoc.a:twai_hal_iram.*) .iram.data.*)
    *libsoc.a:twai_hal_iram.*( .iram.data  .iram.data.*)
    *libsoc.a:uart_hal_iram.*( .iram.data  .iram.data.*)

    _iram_data_end = ABSOLUTE(.);
  } > iram0_0_seg

  .iram0.bss (NOLOAD) :
  {
    . = ALIGN(4);
    _iram_bss_start = ABSOLUTE(.);

    *(EXCLUDE_FILE(*libsoc.a:uart_hal_iram.* *libsoc.a:twai_hal_iram.*) .iram.bss EXCLUDE_FILE(*libsoc.a:uart_hal_iram.* *libsoc.a:twai_hal_iram.*) .iram.bss.*)
    *libsoc.a:twai_hal_iram.*( .iram.bss  .iram.bss.*)
    *libsoc.a:uart_hal_iram.*( .iram.bss  .iram.bss.*)

    _iram_bss_end = ABSOLUTE(.);
    . = ALIGN(4);
    _iram_end = ABSOLUTE(.);
   } > iram0_0_seg

  /* Marks the end of data, bss and possibly rodata  */
  .dram0.heap_start (NOLOAD) :
  {
    . = ALIGN (8);
    _heap_start = ABSOLUTE(.);
  } > dram0_0_seg

  /** This section will be used by the debugger and disassembler to get more information
    * about raw data present in the code.
    * Indeed, it may be required to add some padding at some points in the code
    * in order to align a branch/jump destination on a particular bound.
    * Padding these instructions will generate null bytes that shall be
    * interpreted as data, and not code by the debugger or disassembler.
    * This section will only be present in the ELF file, not in the final binary
    * For more details, check GCC-212
    */
  .xt.prop 0 :
  {
    KEEP (*(.xt.prop .gnu.linkonce.prop.*))
  }

  .xt.lit 0 :
  {
    KEEP (*(.xt.lit .gnu.linkonce.p.*))
  }
}

ASSERT(((_iram_end - ORIGIN(iram0_0_seg)) <= LENGTH(iram0_0_seg)),
          "IRAM0 segment data does not fit.")

ASSERT(((_heap_start - ORIGIN(dram0_0_seg)) <= LENGTH(dram0_0_seg)),
          "DRAM segment data does not fit.")
