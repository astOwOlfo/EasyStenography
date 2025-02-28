#include <string>
#include <iostream>

extern "C" {
    #include <glib.h>
    #include <libnotify/notify.h>
}

void show_notification(const std::string& message) {
    // Initialize libnotify
    if (!notify_init("MyApp")) {
        std::cerr << "Failed to initialize libnotify" << std::endl;
        return;
    }

    // Create the notification
    NotifyNotification *notification = notify_notification_new(
        "Stenography", // Title
        message.c_str(), // Body
        "dialog-information" // Icon name
    );

    // Set urgency level to CRITICAL to override full-screen
    notify_notification_set_urgency(notification, NOTIFY_URGENCY_CRITICAL);

    // Set as transient to show regardless of full-screen
    notify_notification_set_hint(notification, "transient", g_variant_new_boolean(TRUE));

    // Make the notification resident (persistent)
    notify_notification_set_hint(notification, "resident", g_variant_new_boolean(TRUE));

    // Optional: Set notification to appear above full-screen windows
    notify_notification_set_hint(notification, "above", g_variant_new_boolean(TRUE));

    // Set timeout (in milliseconds), -1 for default, 0 for never expire
    notify_notification_set_timeout(notification, 1000);

    // Show the notification
    if (!notify_notification_show(notification, NULL)) {
        fprintf(stderr, "Failed to show notification\n");
        return;
    }

    // Clean up
    g_object_unref(G_OBJECT(notification));
    notify_uninit();
}