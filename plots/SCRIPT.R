library("tidyverse")

FF_RAW <- read_csv("D:/Projects/WS2026/UFZ-Project/plots/sensitivity_output_SeasFalse_NeigFalse.csv")
TT_RAW <- read_csv("D:/Projects/WS2026/UFZ-Project/plots/sensitivity_output_SeasTrue_NeigTrue.csv")
FT_RAW <- read_csv("D:/Projects/WS2026/UFZ-Project/plots/sensitivity_output_SeasFalse_NeigTrue.csv")
TF_RAW <- read_csv("D:/Projects/WS2026/UFZ-Project/plots/sensitivity_output_SeasTrue_NeigFalse.csv")

TT <- TT_RAW %>%
  mutate(timestep = timestep +1) %>%
  mutate(YEAR = ceiling(timestep / 12),
         MONTH = timestep %% 12) %>%
  mutate(MONTH = ifelse(MONTH == 0, 12, MONTH)) %>%
  mutate(SEASON   = ifelse(MONTH %in% c(2:4), "BREEDING", "REARING")) %>%
  group_by(SEASON, YEAR, NEIG_TRESH, NEIG_DIST) %>%
  summarise(AVER_BREEDING = mean(sum_breeding)) %>%
  tidyr::pivot_wider(names_from = SEASON, values_from = AVER_BREEDING) %>%
  filter(YEAR %in% c(19,20) & (BREEDING != 0 | REARING != 0)) %>%
  mutate(SYNC = ((BREEDING-REARING)/2821)) %>%
  mutate(SYNC = ifelse(SYNC <0 , 0, SYNC)) %>%
  group_by(NEIG_TRESH, NEIG_DIST) %>%
  slice_max(order_by = SYNC, n = 1, with_ties = FALSE) %>%
  mutate(OPTION = "Season + Neighbouring")

TF <- TF_RAW %>%
  mutate(timestep = timestep +1) %>%
  mutate(YEAR = ceiling(timestep / 12),
         MONTH = timestep %% 12) %>%
  mutate(MONTH = ifelse(MONTH == 0, 12, MONTH)) %>%
  mutate(SEASON   = ifelse(MONTH %in% c(2:4), "BREEDING", "REARING")) %>%
  group_by(SEASON, YEAR, NEIG_TRESH, NEIG_DIST) %>%
  summarise(AVER_BREEDING = mean(sum_breeding)) %>%
  tidyr::pivot_wider(names_from = SEASON, values_from = AVER_BREEDING) %>%
  filter(YEAR %in% c(19,20) & (BREEDING != 0 | REARING != 0)) %>%
  mutate(SYNC = ((BREEDING-REARING)/2821)) %>%
  mutate(SYNC = ifelse(SYNC <0 , 0, SYNC)) %>%
  group_by(NEIG_TRESH, NEIG_DIST) %>%
  slice_max(order_by = SYNC, n = 1, with_ties = FALSE) %>%
  mutate(OPTION = "Season")

FT <- FT_RAW %>%
  mutate(timestep = timestep +1) %>%
  mutate(YEAR = ceiling(timestep / 12),
         MONTH = timestep %% 12) %>%
  mutate(MONTH = ifelse(MONTH == 0, 12, MONTH)) %>%
  mutate(SEASON   = ifelse(MONTH %in% c(2:4), "BREEDING", "REARING")) %>%
  group_by(SEASON, YEAR, NEIG_TRESH, NEIG_DIST) %>%
  summarise(AVER_BREEDING = mean(sum_breeding)) %>%
  tidyr::pivot_wider(names_from = SEASON, values_from = AVER_BREEDING) %>%
  filter(YEAR %in% c(19,20) & (BREEDING != 0 | REARING != 0)) %>%
  mutate(SYNC = ((BREEDING-REARING)/2821)) %>%
  mutate(SYNC = ifelse(SYNC <0 , 0, SYNC)) %>%
  group_by(NEIG_TRESH, NEIG_DIST) %>%
  slice_max(order_by = SYNC, n = 1, with_ties = FALSE) %>%
  mutate(OPTION = "Neighbouring") %>%
  rbind(tibble(YEAR = 20, NEIG_TRESH = 46, NEIG_DIST = 26, BREEDING = 300, REARING = 300, SYNC = 0, OPTION = "Neighbouring"))

FF <- FF_RAW %>%
  mutate(timestep = timestep +1) %>%
  mutate(YEAR = ceiling(timestep / 12),
         MONTH = timestep %% 12) %>%
  mutate(MONTH = ifelse(MONTH == 0, 12, MONTH)) %>%
  mutate(SEASON   = ifelse(MONTH %in% c(2:4), "BREEDING", "REARING")) %>%
  group_by(SEASON, YEAR, NEIG_TRESH, NEIG_DIST) %>%
  summarise(AVER_BREEDING = mean(sum_breeding)) %>%
  tidyr::pivot_wider(names_from = SEASON, values_from = AVER_BREEDING) %>%
  filter(YEAR %in% c(19,20) & (BREEDING != 0 | REARING != 0)) %>%
  mutate(SYNC = ((BREEDING-REARING)/2821)) %>%
  mutate(SYNC = ifelse(SYNC <0 , 0, SYNC)) %>%
  group_by(NEIG_TRESH, NEIG_DIST) %>%
  slice_max(order_by = SYNC, n = 1, with_ties = FALSE) %>%
  mutate(OPTION = "Random")
  

DATA <- rbind(TT, TF, FT, FF) %>%
  mutate(OPTION = factor(OPTION, levels = c("Random", "Neighbouring", "Season", "Season + Neighbouring")))

DATA_T <- TT_RAW %>%
  mutate(timestep = timestep +1) %>%
  mutate(YEAR = ceiling(timestep / 12),
         MONTH = timestep %% 12) %>%
  mutate(MONTH = ifelse(MONTH == 0, 12, MONTH)) %>%
  mutate(SEASON   = ifelse(MONTH %in% c(2:4), "BREEDING", "REARING")) %>%
  group_by(SEASON, YEAR, NEIG_TRESH, NEIG_DIST) %>%
  summarise(AVER_BREEDING = mean(sum_breeding)) %>%
  tidyr::pivot_wider(names_from = SEASON, values_from = AVER_BREEDING) %>%
  mutate(SYNC = ((BREEDING-REARING)/2821)) %>%
  mutate(SYNC = ifelse(SYNC <0 , 0, SYNC)) %>%
  mutate(OPTION = "Season + Neighbouring") %>%
  group_by(NEIG_TRESH, NEIG_DIST) %>%
  arrange(YEAR, .by_group = TRUE) %>%
  mutate(cross = SYNC >= .8 & lag(SYNC) < .8) %>%
  filter(cross) %>%
  slice(1) %>%
  select(NEIG_TRESH, NEIG_DIST, YEAR)

SYNC_1 <- ggplot(DATA, aes(x= NEIG_DIST, y = NEIG_TRESH, fill = SYNC))+
  geom_raster()+
  facet_wrap(~OPTION) +
  scale_x_continuous(name = "Interaction") +
  scale_y_continuous(name = "Treshold") +
  scale_fill_gradientn(colours = rev(c("#468892", "#74F3D3","#751C6D",  "#FF3BD5")),
                       name = "Sync.") +
  theme(
    strip.text        = element_text(size = 10, color = "grey40", face = "bold"),
    legend.text       = element_text(size = 10, color = "grey40"),
    legend.title      = element_text(margin = margin(0,0,10,0), face = "bold", size = 12, color = "grey40"),
    legend.key.height = unit(6, "mm"),
    legend.key.width  = unit(6, "mm"),
    legend.background = element_rect(fill = "white", color = NA),
    legend.key        = element_rect(fill = "white", color = NA),
    legend.margin     = margin(0,0,0,0),
    plot.margin       = margin(0, 0, 0, 0),
    axis.text         = element_text(size = 10, color = "grey40"),
    axis.ticks        = element_line(color = "grey40"),
    axis.title.x        = element_text(margin = margin(t = 5), face = "bold", size = 12, color = "grey40"),
    axis.title.y        = element_text(margin = margin(r = 10), face = "bold", size = 12, color = "grey40"),
    panel.grid.major  = element_line(linewidth = .4),
    panel.grid.minor  = element_line(size = .2),
    aspect.ratio      = 1
  )

SYNC_2 <- ggplot(DATA_T, aes(x= NEIG_DIST, y = NEIG_TRESH, fill = YEAR))+
  scale_fill_gradientn(colours = rev(c("#468892", "#74F3D3","#751C6D",  "#FF3BD5")), name = "Year") +
  scale_x_continuous(name = "Interaction", breaks = seq(5,25,5)) +
  scale_y_continuous(name = "Treshold") +
  geom_raster() +
  guides(fill = guide_colorbar(reverse = TRUE)) +
  theme(
    legend.text       = element_text(size = 10, color = "grey40"),
    legend.title      = element_text(margin = margin(0,0,10,0), face = "bold", size = 12, color = "grey40"),
    legend.key.height = unit(6, "mm"),
    legend.key.width  = unit(6, "mm"),
    legend.background = element_rect(fill = "white", color = NA),
    legend.key        = element_rect(fill = "white", color = NA),
    legend.margin     = margin(0,0,0,0),
    plot.margin       = margin(0, 0, 0, 0),
    axis.text.y       = element_text(size = 10, color = "grey40"),
    axis.text.x       = element_text(size = 10, color = "grey40"),
    axis.ticks        = element_line(color = "grey40"),
    axis.title.x        = element_text(margin = margin(t = 5), face = "bold", size = 12, color = "grey40"),
    axis.title.y        = element_text(margin = margin(r = 10), face = "bold", size = 12, color = "grey40"),
    panel.grid.major  = element_line(linewidth = .4),
    panel.grid.minor  = element_line(size = .2),
    aspect.ratio      = 1
  )

ggsave(SYNC_1, filename= "D:/Projects/WS2026/UFZ-Project/plots/SYNC_1.png", width= 174, height= 174, unit= "mm", dpi = 600)
ggsave(SYNC_2, filename= "D:/Projects/WS2026/UFZ-Project/plots/SYNC_2.png", width= 174, height= 174, unit= "mm", dpi = 600)
             
             