/*
 * Copyright (C) 2013 Canonical Ltd
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authors:
 * Sebastien Bacher <sebastien.bacher@canonical.com>
 *
 */

function buildDisplayName(sound) {
    /* The display name starts with an uppercase char, and replace special chars with spaces */
    var title = sound.split('/').pop().split('.').slice(0,-1).join(" ").replace(/[._-]/g, " ")
    return title[0].toUpperCase() + title.slice(1)
}

function buildSoundValues(sounds) {
    return sounds.map(buildDisplayName)
}

function indexSelectedFile(soundsList, soundName) {
    return soundsList.indexOf(soundName)
}
