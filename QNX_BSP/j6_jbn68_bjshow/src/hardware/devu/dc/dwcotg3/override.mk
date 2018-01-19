TYPE = $(firstword $(filter a o dll, $(VARIANTS)) o)
BOARD = $(firstword $(filter omap543x msm8974 exynos5410smdk musket, $(VARIANTS)))
USEFILE_dll = $(SECTION_ROOT)/$(BOARD)/$(PROTOCOL)/$(NAME)-$(PROTOCOL)-$(BOARD).use
USEFILE   = $(USEFILE_$(TYPE))
