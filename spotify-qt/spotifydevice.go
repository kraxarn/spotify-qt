package main

type SpotifyDevice struct {
	ID, Name, Type string
	IsActive, IsPrivateSession, IsRestricted bool
	VolumePercent uint8
}