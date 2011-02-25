/* This file is part of the Nepomuk Project
   Copyright (c) 2008-2009 Sebastian Trueg <trueg@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef _NEPOMUK_ANNOTATION_H_
#define _NEPOMUK_ANNOTATION_H_

#include <QtCore/QObject>
#include <QtCore/QDateTime>

#include <Soprano/Error/ErrorCache>

#include "nepomukannotation_export.h"
#include <nepomuk/resource.h>

class QIcon;
class QUrl;

namespace Nepomuk {

    class AnnotationPrivate;

    /**
     * \brief The base class for all Nepomuk annotation actions
     *
     * An Annotation represents a possible annotation that can be
     * created by calling create(). One should also consider reimplementing
     * equals() to improve the user experience.
     *
     * \author Sebastian Trueg <trueg@kde.org>
     *
     * \sa SimpleAnnotation
     */
    class NEPOMUKANNOTATION_EXPORT Annotation : public QObject, public Soprano::Error::ErrorCache
    {
        Q_OBJECT

    public:
        /**
         * Destructor
         */
        virtual ~Annotation();

        /**
         * A short label to show to the user.
         *
         * The default implementation returns the value set
         * via setLabel.
         *
         * FIXME: remove this
         */
        virtual QString label() const;

        /**
         * A longer description to show to the user.
         *
         * The default implementation returns the value set
         * via setComment.
         *
         * FIXME: rename to something else and maybe add something like "question" which
         * would then return a question like "Should foobar be tagged with XYZ?"
         */
        virtual QString comment() const;

        /**
         * An icon to display to the user.
         *
         * The default implementation returns the value
         * set via setIcon.
         */
        virtual QIcon icon() const;

        /**
         * The relevance of the annotation between 0 and 1.
         *
         * The relevance should not take statistics into account. See
         * occurenceCount, lastUsed for statistics.
         *
         * The default implementation returns the relevance set via
         * setRelevance.
         */
        virtual qreal relevance() const;

        /**
         * Comparision of two annotations. This is used to prevent proposing the
         * same annotation to the user multiple times in case more than one plugin
         * creates the same annotation.
         *
         * This should be reimplemented to compare annotations of the same type.
         *
         * Make sure to always call the base implementation in case comparision is
         * not possible (unknown other annotation.
         */
        virtual bool equals( Annotation* other ) const;

        /**
         * Checks if the annotation does already exist for the given resource \a res.
         *
         * \sa AnnotationRelevanceFilterModel
         */
        virtual bool exists( Resource res ) const = 0;

        /**
         * The usage count of this annotation, i.e. how often resources
         * have been annotated like this in a certain time frame.
         *
         * \param from Start of the time frame. Use invalid QDateTime for no
         * limit.
         * \param to End time of the time frame. Use invalid QDateTime for no
         * limit.
         *
         * \return The occurence count of this annotation or -1 if not supported.
         *
         * The default implementation returns -1.
         *
         * Use SimpleAnnotation to have automatic occurence count support.
         */
        virtual int occurenceCount( const QDateTime& from = QDateTime(), const QDateTime& to = QDateTime() ) const;

        /**
         * When was this annotation last used?
         *
         * \return The time of the last usage of this annotation, i.e. when
         * a resource was last annotated like this, or an invalid QDateTime
         * if not supported.
         *
         * The default implementation returns an invalid QDateTime.
         */
        virtual QDateTime lastUsed() const;

        /**
         * When was this annotation first used?
         *
         * \return The time of the first usage of this annotation, i.e. when
         * a resource was first annotated like this, or an invalid QDateTime
         * if not supported.
         *
         * The default implementation returns an invalid QDateTime.
         */
        virtual QDateTime firstUsed() const;

    public Q_SLOTS:
        /**
         * Create the actual annotation.
         *
         * When done the finished signal is emitted.
         *
         * \param res The resource for which the annotation should be created. In most
         * cases this will be the same as in the AnnotationRequest passed to
         * AnnotationPlugin::getPossibleAnnotations
         */
        void create( Resource res );

        /**
         * Set the user label.
         * \sa label()
         *
         * This method is intended for Annotation setup and should not
         * be called by a client.
         */
        void setLabel( const QString& label );

        /**
         * Set the user comment.
         * \sa comment()
         *
         * This method is intended for Annotation setup and should not
         * be called by a client.
         */
        void setComment( const QString& comment );

        /**
         * Set an icon to show to the user.
         * \sa icon()
         *
         * This method is intended for Annotation setup and should not
         * be called by a client.
         */
        void setIcon( const QIcon& icon );

        /**
         * Set the relevance of the annotation.
         * The relevance should be based on the current context.
         *
         * The default relevance is 1.0.
         *
         * \param r The new relevance. This value will be forced into
         * the range 0.0-1.0.
         *
         * This method is intended for Annotation setup and should not
         * be called by a client.
         */
        void setRelevance( qreal r );

        /**
         * Set the relevance as seen by the user, i.e. give feedback for learning
         * plugins that asjust their relevance levels according to user feedback.
         *
         * Typical usage is to set the user relevance to 0 or 1, depending on feedback.
         *
         * Reimplement this method to support learning in the plugin. A typical
         * example would be to remember the times an annotation was rated with a low
         * user relevance and adjust the relevance accordingly in the future.
         *
         * The default implementation does nothing.
         */
        virtual void setUserRelevance( qreal r );

    Q_SIGNALS:
#ifndef Q_MOC_RUN
    private: // don't tell moc, but this signal is in fact private
#endif
        /**
         * Emitted once the creation of the annotation is done.
         * Use Soprano::Error::ErrorCache to check for success.
         *
         * This is a private signal, it can't be emitted directly
         * by subclasses of Annotation, use emitFinished() instead.
         *
         * \param annotation The annotation emitting the signal.
         */
        void finished( Nepomuk::Annotation* annotation );

    protected:
        /**
         * Create a new Annotation.
         */
        Annotation( QObject* parent );

        /**
         * Called by create to do the actual work.
         *
         * Subclasses should use Soprano::Error::ErrorCache::setError
         * to inform avout the success of the operation.
         *
         * Once done the finished signal has to be emitted.
         *
         * The Annotation deletes itself once it has been created.
         */
        virtual void doCreate( Resource res ) = 0;

    protected Q_SLOTS:
        /**
         * Emits the finished signal.
         */
        void emitFinished();

    private:
        AnnotationPrivate* const d;

        friend class AnnotationModel;
        friend class AnnotationModelPrivate;
    };
}

#endif
