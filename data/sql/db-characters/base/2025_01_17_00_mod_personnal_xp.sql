CREATE TABLE IF NOT EXISTS `personnalxp` (
  `CharacterGUID` int(11) NOT NULL,
  `XPRate` FLOAT NOT NULL DEFAULT '1',
  PRIMARY KEY (`CharacterGUID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
