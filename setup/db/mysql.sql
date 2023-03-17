/*
 Fulltext index database
*/

#CREATE DATABASE fulltext_index;
#USE fulltext_index;

/* 
 Example how to create a user:
 CREATE USER 'index'@'localhost' IDENTIFIED BY '<insert-password-here>';
 GRANT ALL PRIVILEGES ON fulltext_index.* to 'index'@'localhost';
*/

CREATE TABLE `DOCUMENT`
(
    `ID` int(11) NOT NULL,
    PRIMARY KEY (`ID`)
) ENGINE MyISAM
  DEFAULT CHARSET = utf8mb4;

CREATE TABLE `WORD`
(
    `ID`    int(11) NOT NULL AUTO_INCREMENT,
    `WORD`  varchar(254) DEFAULT NULL,
    `COUNT` int(11)      DEFAULT 0,
    PRIMARY KEY (`ID`),
    KEY `IDX_WORD` (`WORD`(250))
) ENGINE MyISAM
  DEFAULT CHARSET = utf8mb4;

CREATE TABLE `DOCUMENT_WORD`
(
    `DOCUMENT` int(11) NOT NULL,
    `WORD`     int(11) NOT NULL,
    `COUNT`    int(11) DEFAULT 0,
    PRIMARY KEY (`DOCUMENT`, `WORD`),
    KEY `IDX_DOCUMENT_WORD` (`WORD`)
) ENGINE MyISAM
  DEFAULT CHARSET = utf8mb4;
