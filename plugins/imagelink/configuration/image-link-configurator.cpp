/*
 * %kadu copyright begin%
 * Copyright 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * %kadu copyright end%
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "configuration/configuration-file.h"

#include "configuration/image-link-configuration.h"
#include "image-expander-dom-visitor-provider.h"
#include "video-expander-dom-visitor-provider.h"

#include "image-link-configurator.h"

ImageLinkConfigurator::ImageLinkConfigurator(QObject *parent)
{
	Q_UNUSED(parent);

	createDefaultConfiguration();
}

ImageLinkConfigurator::~ImageLinkConfigurator()
{
}

void ImageLinkConfigurator::setImageExpanderProvider(ImageExpanderDomVisitorProvider *imageExpander)
{
	ImageExpander = imageExpander;
}

void ImageLinkConfigurator::setVideoExpanderProvider(VideoExpanderDomVisitorProvider *videoExpander)
{
	VideoExpander = videoExpander;
}

void ImageLinkConfigurator::configure()
{
	configurationUpdated();
}

void ImageLinkConfigurator::createDefaultConfiguration()
{
	config_file.addVariable("Imagelink", "show_image", true);
	config_file.addVariable("Imagelink", "show_yt", true);
}

void ImageLinkConfigurator::configurationUpdated()
{
	ImageLinkConfiguration configuration;
	configuration.setShowImages(config_file.readBoolEntry("Imagelink", "show_image", true));
	configuration.setShowVideos(config_file.readBoolEntry("Imagelink", "show_yt", true));

	if (ImageExpander)
		ImageExpander.data()->setConfiguration(configuration);
	if (VideoExpander)
		VideoExpander.data()->setConfiguration(configuration);
}