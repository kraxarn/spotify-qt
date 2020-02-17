package main

import "github.com/therecipe/qt/core"

// Slightly less annoying wrapper around QSettings
// Also, easier later if we want to switch, cache, etc.
type Settings struct {
	settings *core.QSettings
}

func NewSettings() *Settings {
	set := new(Settings)
	set.settings = core.NewQSettings5(nil)
	return set
}

func (set *Settings) Get(key string, defaultValue interface{}) interface{} {
	return set.settings.Value(key, core.NewQVariant1(defaultValue)).ToInterface()
}

func (set *Settings) Set(key string, value interface{}) {
	set.settings.SetValue(key, core.NewQVariant1(value))
}

func (set *Settings) Sync() {
	set.settings.Sync()
}